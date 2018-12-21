#pragma once
#ifndef GPIOINTERRUPT_H
#define GPIOINTERRUPT_H
#include <stdint.h>

enum InterruptCondition{
	kRisingEdge,
	kFallingEdge,
	kBothEdges
};

typedef void (*IsrPointer)(void);

class GpioInterrupt {
private:
	IsrPointer pin_isr_map[2][32];
public:
	GpioInterrupt();
	void Initialize();
	bool AttachInterruptHandler(uint8_t port, uint32_t pin, IsrPointer pin_isr, InterruptCondition condition);
	void HandleInterrupt();
};
#endif
