#include "MP3GPIO.hpp"

MP3GPIO::MP3GPIO()
{

}

void MP3GPIO::initialize(uint8_t pin, uint8_t port)
{
	//set both the port and pin
    portNum = port;
    pinNum = pin;
    //Make tthe class generic for all ports.
    switch(port){
        case 0:
            baseGPIO = LPC_GPIO0;
            break;
        case 1:
            baseGPIO = LPC_GPIO1;
            break;
        case 2:
            baseGPIO = LPC_GPIO2;
            break;
        case 3:
            baseGPIO = LPC_GPIO3;
            break;
        case 4:
            baseGPIO = LPC_GPIO4;
            break;
    }
}
    /**
     * Should alter the hardware registers to set the pin as an input
     */
void MP3GPIO::setAsInput()
{
    baseGPIO->FIODIR &= ~(1 << pinNum);
}
    /**
     * Should alter the hardware registers to set the pin as an input
     */
void MP3GPIO::setAsOutput()
{
    baseGPIO->FIODIR |= (1 << pinNum);
}
    /**
     * Should alter the set the direction output or input depending on the input.
     *
     * @param {bool} output - true => output, false => set pin to input
     */
void MP3GPIO::setDirection(bool output)
{
    if(output)
    {
        setAsOutput();
    }
    else
    {
        setAsInput();
    }
}
/**
    * Should alter the hardware registers to set the pin as high
    */
void MP3GPIO::setHigh()
{
    baseGPIO->FIOSET = (1 << pinNum);
}
/**
    * Should alter the hardware registers to set the pin as low
    */
void MP3GPIO::setLow()
{
    baseGPIO->FIOCLR = (1 << pinNum);
}
/**
    * Should alter the hardware registers to set the pin as low
    *
    * @param {bool} high - true => pin high, false => pin low
    */
void MP3GPIO::set(bool high)
{
    if(high)
    {
        setHigh();
    }
    else
    {
        setLow();
    }
}
/**
    * Should return the state of the pin (input or output, doesn't matter)
    *
    * @return {bool} level of pin high => true, low => false
    */
bool MP3GPIO::getLevel()
{
    return (baseGPIO->FIOPIN & (1 << pinNum));
}
MP3GPIO::~MP3GPIO()
{

}
