#include "FreeRTOS.h"
#include "tasks.hpp"
#include "task.h"
#include <stdio.h>
#include "stdlib.h"
#include <iostream>
#include <string>
#include "i2c2.hpp"
#include "light_sensor.hpp"
#include "storage.hpp"
#include <string.h>
#include <rtc.h>
#include "queue.h"
#include "handlers.hpp"
#include "event_groups.h"
#include "printf_lib.h"

QueueHandle_t sensorQ = xQueueCreate(1, sizeof(int));
EventGroupHandle_t sensorEventGroup = xEventGroupCreate();
TaskHandle_t prodHandle = NULL;
TaskHandle_t consHandle = NULL;

//made changes to terminal.cpp and handlers.hpp to create new terminal commands
//created terminal commands for starting and pausing producer task
CMD_HANDLER_FUNC(taskCtrl) {
    //scalable implementation
    // get command, which should be either start or pause
    str command = cmdParams.subString(0, cmdParams.firstIndexOf(" ") + 1);
    output.printf("\nCommand issued: %s\n", cmdParams);

    // get task name
    str taskName = cmdParams.subString(cmdParams.lastIndexOf(" "));
    output.printf("\nTask selected: %s\n", taskName);

    // get task handle for starting and pausing
    char handleStr[10];
    for (uint16_t i = 0; i < taskName.getLen(); ++i) {
        handleStr[i] = taskName[i];
    }

    TaskHandle_t handleTask = xTaskGetHandle(handleStr);

    if (command == "start") {
        vTaskResume(handleTask);
    }
    else if (command == "pause") {
        vTaskSuspend(handleTask);
    }

    //hard coded implementation
    if (cmdParams == "resume producer") {
        vTaskResume(prodHandle);
    }
    else if (cmdParams == "suspend producer") {
        vTaskSuspend(prodHandle);
    }
    else if (cmdParams == "resume consumer") {
        vTaskResume(consHandle);
    }
    else if (cmdParams == "suspend consumer") {
        vTaskSuspend(consHandle);
    }
    return true;
}

void producer(void* pvParam) {
    //get instance using friend function of LightSensor, since it's a singleton
    Light_Sensor& LS = Light_Sensor::getInstance();

    while(1) {
        uint32_t lightval = 0;

        for(uint16_t i = 0; i < 100; ++i) {
            //get light sensor readings
            lightval += LS.getRawValue();

            if (i == 99) {
                lightval = lightval / 100;
                xQueueSend(sensorQ, &lightval, portMAX_DELAY);
                u0_dbg_printf("Inside producer and sent queue\n");
            }
        }
        //set bit flag for producer task, once done
        xEventGroupSetBits(sensorEventGroup, 0x1);
        vTaskDelay(1000);
    }
}

void consumer(void* pvParam) {
    uint32_t lightval = 0;
    uint32_t time = 0;
    char stringTimer[30];
    char readingsHeader[] = "Time \t\t\t Average Light Sensor Value\n";

    Storage::append("1:sensorReadings.txt", &readingsHeader, strlen(readingsHeader), 0);

    while(1) {
        u0_dbg_printf("Inside consumer\n");

        if(xQueueReceive(sensorQ, &lightval, portMAX_DELAY)) {
            //get system's real time clock from rtc.h
            time = LPC_RTC->SEC;
            u0_dbg_printf("Received average is %d\n", lightval);
            sprintf(stringTimer,"%u \t\t\t %u\n", time, lightval);
            Storage::append("1:sensorReadings.txt", &stringTimer, strlen(stringTimer), 0);
        }
        //set bit flag for consumer task, once done
        xEventGroupSetBits(sensorEventGroup, 0x2);
        vTaskDelay(1000);
    }
}

void watcher(void* pvParam) {
    EventBits_t sensorBit = 0;
    char strProducer[] = "Producer is Stuck\n";
    char strConsumer[] = "Consumer is Stuck\n";

    while(1) {
        //monitor producer and consumer task, thus the 0x3
        //1st pdTrue: clears any set bits before EventGroupWaitBits returns
        //2nd pdTrue: to either wait until all task bits are set, or when timer expires
        //set TicksToWait to 1 second
        sensorBit = xEventGroupWaitBits(sensorEventGroup, 0x3, pdTRUE, pdTRUE, 1000);

        if ((sensorBit & 0x1) == 0) {
            Storage::append("1:stuck.txt", &strProducer, strlen(strProducer), 0);
        }
        else if ((sensorBit & 0x2) == 0) {
            Storage::append("1:stuck.txt", &strConsumer, strlen(strConsumer), 0);
        }
    }
}

int main(void) {
    xTaskCreate(watcher, "watchTask", 1024, 0, PRIORITY_HIGH, 0);
    xTaskCreate(producer, "producer", 1024, 0, PRIORITY_MEDIUM, &prodHandle);
    xTaskCreate(consumer, "consumer", 1024, 0, PRIORITY_MEDIUM, &consHandle);

    //terminal task to start and suspend producer and consumer tasks
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_start();

    vTaskStartScheduler();

    return 0;
}