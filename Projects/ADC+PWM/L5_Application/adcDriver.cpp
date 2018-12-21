#include <stdio.h>
#include "LPC17xx.h"
#include "io.hpp"
#include "adcDriver.h"
#include "printf_lib.h"
#include <stdio.h>

LabAdc::LabAdc(){}

void LabAdc::AdcInitBurstMode(){
	//enable A/D converter (ADC) clock control bit
	LPC_SC->PCONP |= (1 << 12);
	//clear and set peripheral clock
	LPC_SC->PCLKSEL0 &= ~(3 << 24);
	LPC_SC->PCLKSEL0 |= (1 << 24);

	LPC_ADC->ADINTEN &= ~(1 << 8);
	LPC_ADC->ADCR &= ~(0x7 << 24);

	//power up PDN (power delivery network) to enable ADC
	LPC_ADC->ADCR |= (1 << 21);

	//set ADC channel for light sensor and enable burst mode
	LPC_ADC->ADCR |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
	LPC_ADC->ADCR = (1 << 16);
}
	
/**
* 1) Selects ADC functionality of any of the ADC pins that are ADC capable
* @param pin is the LabAdc::Pin enumeration of the desired pin.
*
* WARNING: For proper operation of the SJOne board, do NOT configure any pins
* as ADC except for 0.26, 1.31, 1.30
*/
void LabAdc::AdcSelectPin(Pin pin){
	if (pin == k0_25) {
		u0_dbg_printf("inside k0_25\n");
	 	LPC_PINCON->PINSEL1 |= (1 << 18);
		LPC_PINCON->PINSEL1 &= ~(1 << 19);
	}
	else if (pin == k0_26) {
		u0_dbg_printf("inside k0_26\n");
		LPC_PINCON->PINSEL1 |= (1 << 20);
		LPC_PINCON->PINSEL1 &= ~(1 << 21);
	}
	else if (pin == k1_30) {
		u0_dbg_printf("inside k1_30\n");
	 	LPC_PINCON->PINSEL3 |= (3 << 28);
	}
	else if (pin == k1_31) {
		u0_dbg_printf("inside k1_31\n");
		LPC_PINCON->PINSEL3 |= (3 << 30);
	}
}

// 1) Returns the voltage reading of the 12bit register of a given ADC channel
// @param channel is the number (0 through 7) of the desired ADC channel
float LabAdc::ReadAdcVoltageByChannel(uint8_t channel){
	float val;
	if (channel == 2) {
		val = ((LPC_ADC->ADDR2 >> 4) & 0xFFFF);
		u0_dbg_printf("channel 2 volt: %f\n", val);
	}
	else if (channel == 3) {
		val = ((LPC_ADC->ADDR3 >> 4) & 0xFFFF);
		u0_dbg_printf("channel 3 volt: %f\n", val);
	}
	else if (channel == 4) {
		val = ((LPC_ADC->ADDR4 >> 4) & 0xFFFF);
		u0_dbg_printf("channel 4 volt: %f\n", val);
	}
	else if (channel == 5) {
		val = ((LPC_ADC->ADDR5 >> 4) & 0xFFFF);
		u0_dbg_printf("channel 5 volt: %f\n", val);
	}
	return val;
}
