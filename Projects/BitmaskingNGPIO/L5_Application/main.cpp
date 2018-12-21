#include "labgpio.h"
#include "disp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"

bool pressed = false;
bool btn1 = false;
bool btn2 = false;
bool btn3 = false;
bool rst = false;

void vControlLED(void *pvParameters) {
	uint32_t param = (uint32_t)(pvParameters);

	if (param == 0){
		LabGPIO_1 out = LabGPIO_1(1, 19);
		out.setDirection(true);
		bool toggleLED = false;

		while(1){
			toggleLED ? out.setLow() : out.setHigh();
			if (pressed){
				toggleLED = toggleLED ? false : true;
				pressed = false;
			}
			vTaskDelay(250);
		}
		vTaskDelete(NULL);
	}
	else if (param == 1){
		LabGPIO_1 out = LabGPIO_1(1, 1);
		out.setAsOutput();
		bool toggleLED = false;
		
		while(1){
			toggleLED ? out.setLow() : out.setHigh();
			if (pressed){
				toggleLED = toggleLED ? false : true;
				pressed = false;
			}
			vTaskDelay(250);
		}
		vTaskDelete(NULL);
	}
	else {
		LabGPIO_1 out1 = LabGPIO_1(1, 0);
		LabGPIO_1 out2 = LabGPIO_1(1, 1);
		LabGPIO_1 out3 = LabGPIO_1(1, 4);
		LabGPIO_1 out4 = LabGPIO_1(1, 8);
		out1.setAsOutput();
		out2.setAsOutput();
		out3.setAsOutput();
		out4.setAsOutput();
		TickType_t xLastWakeTime = xTaskGetTickCount();

		while(1){
			while(btn1){
				//G
				for(uint8_t i = 0; i < 2; ++i){
					out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//I
				for(uint8_t i = 0; i < 2; ++i){
					out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//V
				for(uint8_t i = 0; i < 3; ++i){
					out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//E
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
			}
			while(btn2){
				//M
				for(uint8_t i = 0; i < 2; ++i){
					out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//E
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
			}
			while(btn3){
				//P
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh(); out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				for(uint8_t i = 0; i < 2; ++i){
					out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//O
				for(uint8_t i = 0; i < 3; ++i){
					out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//I
				for(uint8_t i = 0; i < 2; ++i){
					out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//N
				out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//T
				out1.setLow();	out2.setLow();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
				//S
				for(uint8_t i = 0; i < 3; ++i){
					out1.setLow();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
					out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
					vTaskDelayUntil(&xLastWakeTime, (1000/3));
				}
				//Stop
				out1.setLow();	out2.setLow();	out3.setLow();	out4.setLow();
				vTaskDelayUntil(&xLastWakeTime, (1000/3));
			}
			if(rst){
				out1.setHigh();	out2.setHigh();	out3.setHigh();	out4.setHigh();
			}
			vTaskDelay(500);
		}
		vTaskDelete(NULL);
	}
}

void vReadSwitch(void *pvParameters){
	uint32_t param = (uint32_t)(pvParameters);

	if (param == 0){
		LabGPIO_1 in = LabGPIO_1(1, 20);
		in.setDirection(false);
		bool toggleDisp = false;
		while (1){
			toggleDisp ? uart0_puts("0") : uart0_puts("1");
			if (in.getLevel()){
				pressed = true;
				toggleDisp = toggleDisp ? false : true;
			}
			vTaskDelay(250);
		}
		vTaskDelete(NULL);
	}
	else if (param == 1){
		LabGPIO_1 in = LabGPIO_1(1, 10);
		in.setAsInput();
		bool toggleDisp = false;
		while (1){
			toggleDisp ? uart0_puts("1") : uart0_puts("0");
			if (in.getLevel()){
				pressed = true;
				toggleDisp = toggleDisp ? false : true;
			}
			vTaskDelay(250);
		}
		vTaskDelete(NULL);
	}
	else{
		LabGPIO_1 in1 = LabGPIO_1(1, 9);
		LabGPIO_1 in2 = LabGPIO_1(1, 14);
		LabGPIO_1 in3 = LabGPIO_1(1, 15);
		LabGPIO_1 rest = LabGPIO_1(1, 10);
		in1.setAsInput();
		in2.setAsInput();
		in3.setAsInput();
		rest.setAsInput();
		Disp d = Disp();
		while(1){
			if (btn1){
				d.line(2);	//G
				d.dot(1);		
				d.end();	//end letter
				d.dot(2);	//I
				d.end();	//end letter
				d.dot(3);	//V
				d.line(1);
				d.end();	//end letter
				d.dot(1);	//E
				d.end();	//end letter
			}
			else if (btn2){
				d.line(2);	//M
				d.end();	//end letter
				d.dot(1);	//E
				d.end();	//end letter
			}
			else if (btn3){
				d.dot(0);	//P
				d.line(2);	
				d.dot(0);
				d.end();	//end letter
				d.line(3);	//O
				d.end();	//end letter
				d.dot(2);	//I
				d.end();	//end letter
				d.line(1);	//N
				d.dot(0);
				d.end();	//end letter
				d.line(1);	//T
				d.end();	//end letter
				d.dot(3);	//S
				d.end();	//end letter
			}
			else
				d.none();
			if (in1.getLevel()){
				btn1 = true;
				btn2 = btn3 = false;
			}
			else if (in2.getLevel()){
				btn2 = true;
				btn1 = btn3 = false;
			}
			else if (in3.getLevel()){
				btn3 = true;
				btn1 = btn2 = false;
			}
			else
				rst = true;
			vTaskDelay(500);
		}
		vTaskDelete(NULL);
	}
}

int main(int argc, char const *argv[]) {
	const uint32_t STACK_SIZE = 1024;
	TaskHandle_t xHandle = NULL;
	
	//internal
	xTaskCreate(vControlLED, "IntBut", STACK_SIZE, (void*) 1, 1, &xHandle);
	xTaskCreate(vReadSwitch, "IntLED", STACK_SIZE, (void*) 1, 1, &xHandle);
	//external 
	//xTaskCreate(vControlLED, "ExtBut", STACK_SIZE, (void*) 0, 1, &xHandle);
	//xTaskCreate(vReadSwitch, "ExtLED", STACK_SIZE, (void*) 0, 1, &xHandle);
	//extra credit: Morse Code: "Give me points"
	xTaskCreate(vControlLED, "ECBut", STACK_SIZE, (void*) 2, 1, &xHandle);
	xTaskCreate(vReadSwitch, "ECLED", STACK_SIZE, (void*) 2, 1, &xHandle);

	vTaskStartScheduler();
	
	return 0;
}
