#include "ADCDrv.hpp"

ADCDrv::ADCDrv(){}

/**
* 1) Powers up ADC peripheral
* 2) Set peripheral clock
* 2) Enable ADC
* 3) Select ADC channels
* 4) Enable burst mode
*/

void ADCDrv::AdcInitBurstMode()
{
    uart0_puts("BEG of INIT");
    LPC_SC->PCONP |= (1 << 12);   //set bit to power ADC with PCADC
    LPC_ADC->ADCR |= (1 << 21);    //enable ADC with PDN
    LPC_SC->PCLKSEL0 |= (1 << 24); //select PCLK_ADC to divide CCLK/8

    uart0_puts("MID INIT");
    //Scale clock (Currently)
    LPC_ADC->ADCR |= (1 << 8);
    LPC_ADC->ADCR |= (1 << 16);  //enable burst mode
    uart0_puts("End of INIT");
}

void ADCDrv::AdcSelectPin(Pin pin)
{
    //Need PINSEL1 (bits 01) and PINSEL3 (11) to be able to select ad.2-5
    LPC_ADC->ADCR &= ~(0xff);   //clear channel select
    switch(pin)
    {
        case k0_25:
            LPC_ADC->ADCR |= (1 << 2);
            LPC_PINCON->PINSEL1 &= ~(3 << 18);
            LPC_PINCON->PINSEL1 |= (1 << 18);
            break;
        case k0_26:
            LPC_ADC->ADCR |= (1 << 3);
            LPC_PINCON->PINSEL1 &= ~(3 << 20);
            LPC_PINCON->PINSEL1 |= (1 << 20);
            break;
        case k1_30:
            LPC_ADC->ADCR |= (1 << 4);
            LPC_PINCON->PINSEL3 &= ~(3 << 28);
            LPC_PINCON->PINSEL3 |= (3 << 28);
            break;
        case k1_31:
            LPC_ADC->ADCR |= (1 << 5);
            LPC_PINCON->PINSEL3 &= ~(3 << 30);
            LPC_PINCON->PINSEL3 |= (3 << 30);
            break;
    }
    uart0_puts("ADCSelectPin");
}

/**
* 1) Returns the voltage reading of the 12bit register of a given ADC channel
*
* @param channel is the number (0 through 7) of the desired ADC channel.
*/
float ADCDrv::ReadAdcVoltageByChannel(uint8_t channel)
{
    volatile uint32_t * dataReg;
    switch(channel)
    {
        case 2:
            dataReg = &LPC_ADC->ADDR2;
            break;
        case 3:
            dataReg = &LPC_ADC->ADDR3;
            break;
        case 4:
            dataReg = &LPC_ADC->ADDR4;
            break;
        case 5:
            dataReg = &LPC_ADC->ADDR5;
            break;
    }

    return (*dataReg >> 4) & (0xfff);
}