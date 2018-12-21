#include "GpioInterrupt.h"
#include <stddef.h>
#include "LPC17xx.h"

GpioInterrupt::GpioInterrupt(){}

void GpioInterrupt::Initialize(){
	NVIC_EnableIRQ(EINT3_IRQn);
}

bool GpioInterrupt::AttachInterruptHandler(uint8_t port, uint32_t pin, IsrPointer pin_isr, InterruptCondition condition){
	if (port == 1 || port > 2 || port < 0) return false;
	if (pin > 0 || pin > 31) return false;
	if (pin_isr == NULL) return false;

	//set lookup
	port == 2 ? pin_isr_map[1][pin] = pin_isr : pin_isr_map[port][pin] = pin_isr;
	
	//enable interrutps
	if (port == 0){
		if (condition == kRisingEdge)
			LPC_GPIOINT->IO0IntEnR |= (1 << pin);
		else if (condition == kFallingEdge) 
			LPC_GPIOINT->IO0IntEnF |= (1 << pin);
		else {
			LPC_GPIOINT->IO0IntEnR |= (1 << pin);
			LPC_GPIOINT->IO0IntEnF |= (1 << pin);
		}
	}
	else if (port == 2) {
		if (condition == kRisingEdge)
			LPC_GPIOINT->IO2IntEnR |= (1 << pin);
		else if (condition == kFallingEdge)
			LPC_GPIOINT->IO2IntEnF |= (1 << pin);
		else {
			LPC_GPIOINT->IO2IntEnR |= (1 << pin);
			LPC_GPIOINT->IO2IntEnF |= (1 << pin);
		}
	}
	else
		return false;
		
	return true;
}

void GpioInterrupt::HandleInterrupt(){
	if (LPC_GPIOINT->IO0IntStatR) {
		LPC_GPIOINT->IO0IntClr = LPC_GPIOINT->IO0IntStatR;
		pin_isr_map[0][LPC_GPIOINT->IO0IntStatR]();
	}
	else if (LPC_GPIOINT->IO0IntStatF) {
		LPC_GPIOINT->IO0IntClr = LPC_GPIOINT->IO0IntStatF;
		pin_isr_map[0][LPC_GPIOINT->IO0IntStatF]();
	}
	else if (LPC_GPIOINT->IO2IntStatR) {
		LPC_GPIOINT->IO2IntClr = LPC_GPIOINT->IO2IntStatR;
		pin_isr_map[1][LPC_GPIOINT->IO2IntStatR]();
	}
	else {
		LPC_GPIOINT->IO2IntClr = LPC_GPIOINT->IO2IntStatF;
		pin_isr_map[1][LPC_GPIOINT->IO2IntStatF]();
	}
}
