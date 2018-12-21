#include "INTDrv.hpp"
#include "uart0_min.h"
//Make Interrupts enabled for EINT3
//Fill the lookup table with the handlers
//EVERY ISR callback calls the INTDrv class to handle the interrupt with the callbacks associated with it.
INTDrv * INTDrv::m_instance = NULL;
INTDrv::INTDrv(){};

INTDrv* INTDrv::instance()
{
    if(m_instance == NULL){
        m_instance = new INTDrv();
    }
    return m_instance;
}
/**
 * This should configure NVIC to notice EINT3 IRQs; use NVIC_EnableIRQ()
 */
void INTDrv::Initialize()
{
    NVIC_EnableIRQ(EINT3_IRQn);
}

/**
 * This handler should place a function pointer within the lookup table for the HandleInterrupt() to find.
 *
 * @param[in] port         specify the GPIO port, and 1st dimension of the lookup matrix
 * @param[in] pin          specify the GPIO pin to assign an ISR to, and 2nd dimension of the lookup matrix
 * @param[in] pin_isr      function to run when the interrupt event occurs
 * @param[in] condition    condition for the interrupt to occur on. RISING, FALLING or BOTH edges.
 * @return should return true if valid ports, pins, isrs were supplied and pin isr insertion was successful
 */
bool INTDrv::AttachInterruptHandler(uint8_t port, uint32_t pin, IsrPointer pin_isr, InterruptCondition condition)
{
    if(!pin_isr)
    {
        return false;
    }
    if(port != 0 && port != 2)
    {
        return false;
    }
    //TO DO: CHECK VALID PIN #
    //This function places the given callback into the lookup table
    //Configure the appropriate pins to use EINT3 and use a rising/falling edge

    
    volatile uint32_t * IntEnR = (port) ? &LPC_GPIOINT->IO2IntEnR : &LPC_GPIOINT->IO0IntEnR;
    volatile uint32_t * IntEnF = (port) ? &LPC_GPIOINT->IO2IntEnF : &LPC_GPIOINT->IO0IntEnF;

    switch(condition)
    {
        case kRisingEdge:
            *IntEnR |= (1 << pin);
            break;
        case kFallingEdge:
            *IntEnF |= (1 << pin);
            break;
        case kBothEdges:
            *IntEnR |= (1 << pin);
            *IntEnF |= (1 << pin);
            break;
    }

    //we need to attach the ISR callback to the correct lookup table index;
    //uint8_t port_index = (port) ? 1:0;
    pin_isr_map[pin] = pin_isr;
    uart0_puts("interrupt attached\n");
    return true;
}

void INTDrv::HandleInterrupt()
{
    //DECIDE WHICH PIN HAD INTERRUPT
    uint8_t port_index = (LPC_GPIOINT->IntStatus & 1) ? 0:1;
    uint8_t pin_index = 0;

    volatile uint32_t * clearRegister;
    volatile uint32_t * INTRising;
    volatile uint32_t * INTFalling;
    if(port_index)
    {
        clearRegister = &LPC_GPIOINT->IO2IntClr;
        INTRising = &LPC_GPIOINT->IO2IntStatR;
        INTFalling = &LPC_GPIOINT->IO2IntStatF;
    }
    else
    {
        clearRegister = &LPC_GPIOINT->IO0IntClr;
        INTRising = &LPC_GPIOINT->IO0IntStatR;
        INTFalling = &LPC_GPIOINT->IO0IntStatF;
    }

    uint32_t RF_interrupt_pins = *INTRising | *INTFalling;
    //capture
    // while(pin_index != 32)
    // {
    //     if(RF_interrupt_pins & (1 << pin_index))
    //     {
    //         //call the handler associated with this pin
    //         pin_isr_map[port_index][pin_index]();
    //         //clear the INT for this pin
    //         *clearRegister = (1 << pin_index);
    //     }

    //     if((RF_interrupt_pins >> (pin_index+1)) == 0)
    //     {
    //         //nothing else is set
    //         break;
    //     }
    //     ++pin_index;
    // }
    for(uint8_t i =0; i<4; i++)
    {
        if(RF_interrupt_pins & (1 << i))
        {
            // uart0_puts("call isr");
            pin_isr_map[i]();
            *clearRegister = (1 << i);
        }
    }
}
