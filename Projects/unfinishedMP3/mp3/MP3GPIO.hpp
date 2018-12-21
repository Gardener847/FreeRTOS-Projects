#ifndef MP3GPIO_HPP__
#define MP3GPIO_HPP__
#include "LPC17xx.h"
#include <stddef.h>
class MP3GPIO
{
private:
    /**
     * port, pin and any other variables should be placed here.
     * NOTE: that the state of the pin should not be recorded here. The true
     *      source of that information is embedded in the hardware registers
     */
     uint8_t portNum;
     uint8_t pinNum;
     LPC_GPIO_TypeDef * baseGPIO;
public:
    /**
     * You should not modify any hardware registers at this point
     * You should store the port and pin using the constructor.
     *
     * @param {uint8_t} pin  - pin number between 0 and 32
     */
	MP3GPIO(uint8_t pin, uint8_t port);
    /**
     * Should alter the hardware registers to set the pin as an input
     */
	void setAsInput();
    /**
     * Should alter the hardware registers to set the pin as an input
     */
	void setAsOutput();
    /**
     * Should alter the set the direction output or input depending on the input.
     *
     * @param {bool} output - true => output, false => set pin to input
     */
    void setDirection(bool output);
    /**
     * Should alter the hardware registers to set the pin as high
     */
    void setHigh();
    /**
     * Should alter the hardware registers to set the pin as low
     */
    void setLow();
    /**
     * Should alter the hardware registers to set the pin as low
     *
     * @param {bool} high - true => pin high, false => pin low
     */
    void set(bool high);
    /**
     * Should return the state of the pin (input or output, doesn't matter)
     *
     * @return {bool} level of pin high => true, low => false
     */
	bool getLevel();
	~MP3GPIO();
};

#endif