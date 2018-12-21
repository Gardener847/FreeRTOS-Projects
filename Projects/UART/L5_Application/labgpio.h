#pragma once
#ifndef LABGPIO_H
#define LABGPIO_H
#include <stdint.h>

class LabGPIO_1 {
private:
	uint8_t portNum;
	uint8_t pinNum;
public:
	LabGPIO_1(uint8_t port, uint8_t pin);
	void setAsInput();
	void setAsOutput();
	void setDirection(bool output);
	void setHigh();
	void setLow();
	void set(bool high);
	bool getLevel();
	~LabGPIO_1();
};
#endif
