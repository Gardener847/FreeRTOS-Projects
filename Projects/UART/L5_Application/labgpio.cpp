#include "labgpio.h"
#include "LPC17xx.h"

LabGPIO_1::LabGPIO_1(uint8_t port, uint8_t pin) {
	portNum = port;
	pinNum = pin;
}

void LabGPIO_1::setAsInput() {
	if (portNum == 0)	
		LPC_GPIO0->FIODIR &= ~(1 << pinNum);
	else if (portNum == 1)	
		LPC_GPIO1->FIODIR &= ~(1 << pinNum);
	else	
		LPC_GPIO2->FIODIR &= ~(1 << pinNum);
}

void LabGPIO_1::setAsOutput() {
	if (portNum == 0)
		LPC_GPIO0->FIODIR |= (1 << pinNum);
	else if (portNum == 1)
		LPC_GPIO1->FIODIR |= (1 << pinNum);
	else
		LPC_GPIO2->FIODIR |= (1 << pinNum);
}

void LabGPIO_1::setDirection(bool output){
	if (portNum == 0)
		LPC_GPIO0->FIODIR = (output ? (LPC_GPIO0->FIODIR | (1 << pinNum)) : (LPC_GPIO0->FIODIR & ~(1 << pinNum)));
	else if (portNum == 1)
		LPC_GPIO1->FIODIR = (output ? (LPC_GPIO1->FIODIR | (1 << pinNum)) : (LPC_GPIO1->FIODIR & ~(1 << pinNum)));
	else
		LPC_GPIO2->FIODIR = (output ? (LPC_GPIO2->FIODIR | (1 << pinNum)) : (LPC_GPIO2->FIODIR & ~(1 << pinNum)));
}

void LabGPIO_1::setHigh() {
	if (portNum == 0)
		LPC_GPIO0->FIOSET = (1 << pinNum);
	else if (portNum == 1)
		LPC_GPIO1->FIOSET = (1 << pinNum);
	else
		LPC_GPIO2->FIOSET = (1 << pinNum);
}

void LabGPIO_1::setLow() {
	if (portNum == 0)
		LPC_GPIO0->FIOCLR = (1 << pinNum);
	else if (portNum == 1)
		LPC_GPIO1->FIOCLR = (1 << pinNum);
	else
		LPC_GPIO2->FIOCLR = (1 << pinNum);
}

void LabGPIO_1::set(bool high) {
	if (portNum == 0)
		LPC_GPIO0->FIOPIN = (high ? (LPC_GPIO0->FIOPIN | (1 << pinNum)) : (LPC_GPIO0->FIOPIN & ~(1 << pinNum)));
	else if (portNum == 1)
		LPC_GPIO1->FIOPIN = (high ? (LPC_GPIO1->FIOPIN | (1 << pinNum)) : (LPC_GPIO1->FIOPIN & ~(1 << pinNum)));
	else
		LPC_GPIO2->FIOPIN = (high ? (LPC_GPIO2->FIOPIN | (1 << pinNum)) : (LPC_GPIO2->FIOPIN & ~(1 << pinNum)));
}

bool LabGPIO_1::getLevel() {
	if (portNum == 0)
		return (LPC_GPIO0->FIOPIN & (1 << pinNum) ? true : false);
	else if(portNum == 1)
		return (LPC_GPIO1->FIOPIN & (1 << pinNum) ? true : false);
	else
		return (LPC_GPIO2->FIOPIN & (1 << pinNum) ? true : false);
}

LabGPIO_1::~LabGPIO_1() {
	delete &portNum;
	delete &pinNum;
}
