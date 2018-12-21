#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "printf_lib.h"
#include "LPC17xx.h"
#include "adcDriver.h"
#include "pwmDriver.h"

float voltage;
bool toggle;

void btn(void *pvParam){
	LPC_GPIO1->FIODIR &= ~(1 << 9);
	while (1) {
		if (LPC_GPIO1->FIOPIN & (1 << 9)) {
			while(LPC_GPIO1->FIOPIN & (1 << 9));
			toggle = !toggle;
			vTaskDelay(500);
		}
	}
}

void pwm (void *pvParam){
	LabPwm pwm;
	pwm.PwmInitSingleEdgeMode(100);
	pwm.PwmSelectAllPins();
	while (1) {
		if (toggle) {
			pwm.SetDutyCycle(LabPwm::k2_0, voltage);
			pwm.SetDutyCycle(LabPwm::k2_1, voltage/2);
			pwm.SetDutyCycle(LabPwm::k2_2, voltage/3);
		}
		else {
			pwm.SetDutyCycle(LabPwm::k2_0, 0);
			pwm.SetDutyCycle(LabPwm::k2_1, 0);
			pwm.SetDutyCycle(LabPwm::k2_2, 0);
		}
		vTaskDelay(50);	
	}
}

void adc (void *pvParam){
	float volt1, volt2, volt3;
	LabAdc ad;
	ad.AdcInitBurstMode();
	ad.AdcSelectPin(LabAdc::k0_25);
	ad.AdcSelectPin(LabAdc::k0_26);
	ad.AdcSelectPin(LabAdc::k1_30);
	while(1) {
		volt1 = ad.ReadAdcVoltageByChannel(2);
		volt2 = ad.ReadAdcVoltageByChannel(3);
		volt3 = ad.ReadAdcVoltageByChannel(4);
		u0_dbg_printf("volt1: %f\n", volt1);
		u0_dbg_printf("volt2: %f\n", volt2);
		voltage = volt1 * 100 / 3.3;
		u0_dbg_printf("Up time: %d \n", voltage);
		vTaskDelay(1000);
	}
}

int main(int argc, char const *argv[]){
	const uint32_t STACK_SIZE = 1024;
	toggle = true;
	xTaskCreate(adc, "adc", STACK_SIZE, (void*) 0, 1, NULL);
	xTaskCreate(pwm, "pwm", STACK_SIZE, (void*) 0, 1, NULL);
	xTaskCreate(btn, "btn", STACK_SIZE, (void*) 0, 1, NULL);

	vTaskStartScheduler();
	return 0;
}
