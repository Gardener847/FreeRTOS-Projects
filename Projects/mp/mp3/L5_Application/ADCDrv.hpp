#include <stdio.h>
#include "io.hpp"
#include "LPC17xx.h"
#include <stddef.h>
#include "uart0_min.h"
class ADCDrv
{
public:
    enum Pin
    {
        k0_25,       // AD0.2 <-- Light Sensor -->
        k0_26,       // AD0.3
        k1_30,       // AD0.4
        k1_31,       // AD0.5
    };

    ADCDrv();
    
    /**
    * 1) Powers up ADC peripheral
    * 2) Set peripheral clock
    * 2) Enable ADC
    * 3) Select ADC channels
    * 4) Enable burst mode
    */
    void AdcInitBurstMode();

    /**
    * 1) Selects ADC functionality of any of the ADC pins that are ADC capable
    * 
    * @param pin is the ADCDrv::Pin enumeration of the desired pin.
    *
    * WARNING: For proper operation of the SJOne board, do NOT configure any pins
    *           as ADC except for 0.26, 1.31, 1.30
    */
    void AdcSelectPin(Pin pin);
    
    /**
    * 1) Returns the voltage reading of the 12bit register of a given ADC channel
    *
    * @param channel is the number (0 through 7) of the desired ADC channel.
    */
    float ReadAdcVoltageByChannel(uint8_t channel);
};