#include "FreeRTOS.h"
#include "task.h"
// #include <stdio.h>
// #include "printf_lib.h"

#include "Flash.h"

Flash obj;

void vprintMp3Files(void * pvParam) {
	while (1) {
		obj.readMP3Files;
	}
}

int main(int argc, char const *argv[]){
	xTaskCreate(vprintMp3Files, "printMp3Files", 1024, (void*) 0, 1, NULL);

	vTaskStartScheduler();

	while();

	return 0;
}