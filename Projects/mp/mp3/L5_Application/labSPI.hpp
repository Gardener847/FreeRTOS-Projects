#ifndef LABSPI_HPP__
#define LABSPI_HPP__

#include <stdint.h>
#include <stdio.h>
#include "LPC17xx.h"
#include "utilities.h"
#include <stddef.h>
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "uart0_min.h"
#include "io.hpp"
#include "switches.hpp"
#include "gpio.hpp"

class LabSPI
{
 public:
    enum FrameModes
    {
        SPI = 0,
        TI = 1,
        Microwire = 2,
        /* Fill this out based on the datasheet. */
    };

    /**
     * 1) Powers on SPPn peripheral
     * 2) Set peripheral clock
     * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
     *
     * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the datasheet
     * @param format is the code format for which synchronous serial protocol you want to use.
     * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
     *
     * @return true if initialization was successful
     */
    bool initialize()
    {

        LPC_SC->PCONP |= (1 << 21);     //enable power for ssp0
        LPC_SC->PCLKSEL1 &= ~(3 << 10); //select peripheral clock for ssp0
        LPC_SC->PCLKSEL1 |= (2 << 10);  //pcclk = cclck/2

        LPC_PINCON->PINSEL1 &= ~(3 << 2);
        LPC_PINCON->PINSEL0 |= (2 << 30);        //sclk0 P0.15
        LPC_PINCON->PINSEL1 |= (2 << 2);        //MISO0 P0.17
        LPC_PINCON->PINSEL1 |= (2 << 4);        //MOSI0 P0.18

        LPC_SSP0->CR0 = 7;	// how many bits are being transferred
        
        //SSP_PCLK is divided by this even number between 2-254
        //SPI CLOCK NEEDS TO BE 2Mhz to satisfy the decoders specifications
        LPC_SSP0->CPSR = 128;         //clock division
        LPC_SSP0->CR1 |= (1<<1);	//enable SSP 
        

        return true;

    }

    bool initializeDecoderSPI()
    {      
        LPC_SC->PCONP |= (1 << 10);     //enable power for ssp1
        LPC_SC->PCLKSEL0 &= ~(3 << 20);      //select peripheral clock for ssp1
        LPC_SC->PCLKSEL0 |= (2 << 20);  //pcclk = cclck/2

        LPC_PINCON->PINSEL0 &= ~((3 << 14) | (3 << 16) | (3<<18));        //clear the bits
        LPC_PINCON->PINSEL0 |= (2 << 14);        //sclk1 P0.7
        LPC_PINCON->PINSEL0 |= (2 << 16);        //MISO1 P0.8
        LPC_PINCON->PINSEL0 |= (2 << 18);        //MOSI1 P0.9

        LPC_SSP1->CR0 = 7;

        LPC_SSP1->CPSR = 48;
        LPC_SSP1->CR1 |= (1<<1);

        return true;
    }
    /**
     * Transfers a byte via SSP to an external device using the SSP data register.
     * This region must be protected by a mutex static to this class.
     *
     * @return received byte from external device via SSP data register.
     */
    uint8_t transfer(uint8_t send)
    {
        LPC_SSP0->DR = send;
        while(LPC_SSP0->SR & (1 << 4));     //SR = status register
        //while the ssp is busy
        return LPC_SSP0->DR;        //return the byte
    }

    uint8_t transferMP3(uint8_t send)
    {
        LPC_SSP1->DR = send;
        while(LPC_SSP1->SR & (1 << 4));     //SR = status register
        //while the ssp is busy
        return LPC_SSP1->DR;        //return the byte
    }
    LabSPI()
    {

    }
    ~LabSPI()
    {

    }
  
 private:
  
};

#endif