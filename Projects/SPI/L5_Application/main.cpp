#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "printf_lib.h"
#include "LPC17xx.h"
#include "LabSpi.h"
#include "semphr.h"
#include "spi_flash.h"
#include "diskioStructs.h"
#include "integer.h"

SemaphoreHandle_t xSemMutex = NULL;

void vFlashReadSig (void *pvParam) {
	LabSpi obj;
	uint8_t dataSize = 0x7;
	uint8_t divide = 8;
	while (1) {
		if (xSemMutex != NULL) {
			if (xSemaphoreTake(xSemMutex, 1000) == pdTRUE) {
				obj.initialize(dataSize, LabSpi::spi, divide);
				obj.adestoCS();
				obj.readSig();
				obj.adestoDS();
				xSemaphoreGive( xSemMutex );
				vTaskDelay(1000);
			}
		}
	}
}

void vFlashReadStat (void *pvParam) {
	LabSpi obj;
	uint8_t dataSize = 0x7;
	uint8_t divide = 8;
	while (1) {
		if (xSemMutex != NULL) {
			if (xSemaphoreTake(xSemMutex, 1000) == pdTRUE) {
				obj.initialize(dataSize, LabSpi::spi, divide);
				u0_dbg_printf("\n\nStatus After Asking for Device Signature\n");
				obj.adestoCS();
				obj.readFlashStatus();
				obj.adestoDS();
				xSemaphoreGive( xSemMutex );
				vTaskDelay(1000);
			}
		}
	}
}

void vReadBootRecord(void *pvParam) {
	uint8_t dataSize = 0x7;
	uint8_t divide = 8;
	unsigned char *ptr = 0;
	int secNum = 1;
	int secCount = 1;
	while (1) {
		u0_dbg_printf("\ninside EC function\n");
		if (xSemMutex != NULL) {
			u0_dbg_printf("\ninside EC function if\n");
			if (xSemaphoreTake(xSemMutex, 1000) == pdTRUE) {
				flash_initialize();
				u0_dbg_printf("\n\nReading from Master Boot Record Sector\n");
				u0_dbg_printf("-------------------------------------------\n");
				flash_read_sectors(ptr, secNum, secCount);
			// 	if (flash_read_sectors(ptr, secNum, secCount) == DRESULT::RES_OK) {
			// 		u0_dbg_printf("\n%s\n", ptr);
			// 	}
				xSemaphoreGive( xSemMutex );
				vTaskDelay(1000);
			}
		}
	}
}

int main(int argc, char const *argv[]){
	xSemMutex = xSemaphoreCreateMutex();

	xTaskCreate(vFlashReadSig, "flashReadSig", 1024, (void*) 0, 1, NULL);
	xTaskCreate(vFlashReadStat, "flashReadStat", 1024, (void*) 0, 1, NULL);
	// xTaskCreate(vReadBootRecord, "readBootRecord", 1024, (void*) 0, 1, NULL);
	vTaskStartScheduler();

	while(1);

	return 0;
}