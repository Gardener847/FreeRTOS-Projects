#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"

void vTaskOneCode(void *pvParam) {
	while(1) {
		uart0_puts("aaaaaaaaaaaaaaaaaaaa");
		vTaskDelay(100);
	}
}

void vTaskTwoCode(void *pvParam) {
	while(1) {
		uart0_puts("bbbbbbbbbbbbbbbbbbbb");
		vTaskDelay(150);
	}
}

typedef struct A_STRUCT {
	char cStructMem1;
} xStruct;

xStruct xParam = {1};

int main(int argc, char const *argv[]){
	const uint32_t STACK_SIZE = 1024;
	TaskHandle_t xHandle = NULL;

	xTaskCreate(vTaskOneCode, "TaskOne", STACK_SIZE, (void*) &xParam, 1, &xHandle);
	xTaskCreate(vTaskTwoCode, "TaskTwo", STACK_SIZE, (void*) &xParam, 1, &xHandle);

	vTaskStartScheduler();

	return 0;
}
