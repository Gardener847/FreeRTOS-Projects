#ifndef LAB_ADC_H
#define LAB_ADC_H
#include <stdio.h>
#include "io.hpp"

class LabAdc{
public:
	enum Pin {k0_25,	// AD0.2 <-- Light Sensor -->
		k0_26,		// AD0.3
		k1_30,		// AD0.4
		k1_31,		// AD0.5
		/* These ADC channels are compromised on the SJ-ONE
		*  hence you don't need to support them
		*/
		// k0_23 = 0	// ADO.0
		// k0_24,	// ADO.1
		// k0_3,	// ADO.6
		// k0_2		// ADO.7
	};
	// Nothing needs to be done with the default constructor
	LabAdc();

	/**
	* 1) Powers up ADC peripheral 
	* 2) Set peripheral clock
	* 3) Enable ADC
	* 4) Select ADC channels
	* 5) Enable burst mode
	*/
	void AdcInitBurstMode();
	
	/**
	* 1) Selects ADC functionality of any of the ADC pins that are ADC capable
	* @param pin is the LabAdc::Pin enumeration of the desired pin.
	*
	* WARNING: For proper operation of the SJOne board, do NOT configure any pins
	* as ADC except for 0.26, 1.31, 1.30
	*/
	void AdcSelectPin(Pin pin);

	/**
	* 1) Returns the voltage reading of the 12bit register of a given ADC channel
	*
	* @param channel is the number (0 through 7) of the desired ADC channel.
	*/
	float ReadAdcVoltageByChannel(uint8_t channel);
};

#endif
