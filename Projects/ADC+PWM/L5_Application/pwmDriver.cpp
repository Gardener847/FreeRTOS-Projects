#include "pwmDriver.h"
#include "LPC17xx.h"
#include <stdint.h>
#include <stdio.h>
#include "io.hpp"

LabPwm::LabPwm() {}

/**
* 1) Select PWM functionality on all PWM-able pins
*/
void LabPwm::PwmSelectAllPins(){
	LPC_PINCON->PINSEL4 &= ~(0xFFF << 0);
	LPC_PINCON->PINSEL4 |= (0x555 << 0);
}

/**
* 1) Select PWM functionality of pwm_pin_arg
* @param pwm_pin_arg
*/
void LabPwm::PwmSelectPin(PWM_PIN pwm_pin_arg){
	if (pwm_pin_arg == k2_0) {
		LPC_PINCON->PINSEL4 |= (1 << 0);
		LPC_PINCON->PINSEL4 &= ~(1 << 1);
	}
	else if (pwm_pin_arg == k2_1) {
	 	LPC_PINCON->PINSEL4 |= (1 << 2);
		LPC_PINCON->PINSEL4 &= ~(1 << 3);
	}
	else if (pwm_pin_arg == k2_2) {
	 	LPC_PINCON->PINSEL4 |= (1 << 4);
		LPC_PINCON->PINSEL4 &= ~(1 << 5);
	}
	else if (pwm_pin_arg == k2_3) {
		LPC_PINCON->PINSEL4 |= (1 << 6);
		LPC_PINCON->PINSEL4 &= ~(1 << 7);
	}
	else if (pwm_pin_arg == k2_4) {
		LPC_PINCON->PINSEL4 |= (1 << 8);
		LPC_PINCON->PINSEL4 &= ~(1 << 9);
	}
	else if (pwm_pin_arg == k2_5) {
		LPC_PINCON->PINSEL4 |= (1 << 10);
		LPC_PINCON->PINSEL4 &= ~(1 << 11);
	}
}

void LabPwm::PwmInitSingleEdgeMode(uint32_t frequency_Hz){
	LPC_SC->PCONP |= (1 << 6);	//power up
	//clear and set peripheral clock
	LPC_SC->PCLKSEL0 &= ~(1 << 13);
	LPC_SC->PCLKSEL0 |= (1 << 12);
	//disable getting value from TC when capture event occurs, since it's not needed.
	//Possibly reduces additional overhead. It uses bits 5:0
	LPC_PWM1->CCR &= ~(0xF << 0);
	//set match control register function to reset when match register equals timer counter (TC)
	//each MCR is given 3 functions for each bit given 3 bits per match register number (MRn)
	//bit 0 is interrupt, bit 1 is reset, and bit 2 is stop
	LPC_PWM1->MCR |= (1 << 1);
	//select single edge PWM by setting bits 6:2 to 0. 1:0 are unusued. Edge can be either
	//set frequency
	SetFrequency(frequency_Hz);
	//enable counter and PWM
	LPC_PWM1->TCR |= ((1 << 0)| (1 << 3));
	//leading or trailing
	LPC_PWM1->PCR &= ~(0x1F << 2);
	//enable all output channels for bits 14:9 
	LPC_PWM1->PCR |= (0x3F << 9);
	//set 0% duty cycle
	SetDutyCycle(k2_0, 0);
	SetDutyCycle(k2_1, 0);
	SetDutyCycle(k2_2, 0);
	SetDutyCycle(k2_3, 0);
	SetDutyCycle(k2_4, 0);
	SetDutyCycle(k2_5, 0);
}
	
/**
* 1) Convert duty_cycle percentage to the appropriate match register value (depends on current frequency)
* 2) Assign the above value to the appropriate MRn register (depends on pwm_pin_arg)
*
* @param pwn_pin_arg is the PWM_PIN enumeration of the desired pin.
* @param duty_cycle_percentage is the desired duty cycle percentage.
*/
void LabPwm::SetDutyCycle(PWM_PIN pwm_pin_arg, float duty_cycle_percentage){
	uint32_t val = LPC_PWM1->MR0 * duty_cycle_percentage / 100;

	if (pwm_pin_arg == k2_0) {
		LPC_PWM1->MR1 = val;
		LPC_PWM1->LER |= (1 << 1);
	}
	else if (pwm_pin_arg == k2_1) {
		LPC_PWM1->MR2 = val;
		LPC_PWM1->LER |= (1 << 2);
	}
	else if (pwm_pin_arg == k2_2) {
		LPC_PWM1->MR3 = val;
		LPC_PWM1->LER |= (1 << 3);
	}
	else if (pwm_pin_arg == k2_3) {
		LPC_PWM1->MR4 = val;
		LPC_PWM1->LER |= (1 << 4);
	}
	else if (pwm_pin_arg == k2_4) {
		LPC_PWM1->MR5 = val;
		LPC_PWM1->LER |= (1 << 5);
	}
	else if (pwm_pin_arg == k2_5) {
		LPC_PWM1->MR6 = val;
		LPC_PWM1->LER |= (1 << 6);
	}
}

/**
* Optional:
* 1) Convert frequency_Hz to the appropriate match register value
* 2) Assign the above value to MR0
*
* @param frequency_hz is the desired frequency of all pwm pins
*/
void LabPwm::SetFrequency(uint32_t frequency_Hz){
	LPC_PWM1->PC = 0;
	LPC_PWM1->MR0 = sys_get_cpu_clock() / frequency_Hz;
	LPC_PWM1->LER |= (1 << 0);
}
