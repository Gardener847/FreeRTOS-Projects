#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "LPC17xx.h"
#include "LabUART.h"
#include "labgpio.h"
#include "printf_lib.h"
#include "io.hpp"

LabUART obj;

void my_uart2_rx(void *pvParam) {
	while (1) {
		obj.receive();
		vTaskDelay(1000);
		obj.receive();
		vTaskDelay(1000);
		obj.receive();
		vTaskDelay(1000);
	}
}

void my_uart2_tx(void *pvParam) {
	uint8_t c = 3, d = 5;
	while (1) {
		u0_dbg_printf("\nSending operand 1...\n");
		obj.transmit(c);
		vTaskDelay(1000);
		u0_dbg_printf("Sending operand 2...\n");
		obj.transmit(d);
		vTaskDelay(1000);
		u0_dbg_printf("Sending result...\n");
		obj.transmit(c + d);
		vTaskDelay(1000);
	}
}

void btnInput (void *pvParam) {
	// // 9, 10, 14, 15 (buttons)
	LabGPIO_1 op1 = LabGPIO_1(1, 9);
	LabGPIO_1 op2 = LabGPIO_1(1, 10);
	LabGPIO_1 ope = LabGPIO_1(1, 14);
	LabGPIO_1 equ = LabGPIO_1(1, 15);
	op1.setAsInput();
	op2.setAsInput();
	ope.setAsInput();
	equ.setAsInput();

	uint8_t val1 = 0
			, val2 = 0
			, oper = 0;

	while (1) {
		if (op1.getLevel()) {
			val1++;
			LD.setRightDigit(val1);
			obj.transmit(val1);
			vTaskDelay(500);
		}
		else if (op2.getLevel()){
			val2++;
			LD.setRightDigit(val2);
			obj.transmit(val2);
			vTaskDelay(500);
		}
		else if (ope.getLevel()){
			oper++;
			if (oper == 1) {
				LD.setNumber(1);
			}
			else if (oper == 2) {
				LD.setNumber(2);
			}
			else if (oper == 3) {
				LD.setNumber(3);
			}
			else if (oper == 4) {
				LD.setNumber(4);
			}
			vTaskDelay(500);
		}
		else if (equ.getLevel()){
			u0_dbg_printf("\nSending operand 1...\n");
			obj.transmit(val1);
			vTaskDelay(1000);
			u0_dbg_printf("Sending operand 2...\n");
			obj.transmit(val2);
			vTaskDelay(1000);
			
			if (oper == 1) {
				u0_dbg_printf("Sending sum...\n");
				obj.transmit(val1 + val2);
			}
			else if (oper == 2) {
				obj.transmit(val1 - val2);
				if (val1 < val2) {
					LD.setNumber((val2 - val1));
				}
				u0_dbg_printf("Sending difference...\n");
			}
			else if (oper == 3) {
				u0_dbg_printf("Sending product...\n");
				obj.transmit(val1 * val2);
			}
			else if (oper == 4) {
				u0_dbg_printf("Sending quotient...\n");
				obj.transmit(val1 / val2);
			}
			val1 = 0;
			val2 = 0;
			oper = 0;
			vTaskDelay(1000);
		}
	}
}

int main(int argc, char const *argv[]){
	obj.initialize(LabUART::UART2);

	//ec
	xTaskCreate(btnInput, "IntBut", 1024, (void*) 0, 1, NULL);

	// xTaskCreate(my_uart2_tx, "send", 1024, (void*) 0, 1, NULL);
	xTaskCreate(my_uart2_rx, "receive", 1024, (void*) 0, 1, NULL);
	vTaskStartScheduler();

	while(1);

	return 0;
}