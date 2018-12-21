#ifndef MP3SPI_HPP__
#define MP3SPI_HPP__

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

typedef union
{
    uint8_t byte;       //figure out how to assign SR to these bits
struct
	{
		uint8_t tfe: 1;
		uint8_t tnf: 1;
		uint8_t rne: 1;
		uint8_t rff: 1;
		uint8_t bsy: 1;
	} __attribute__((packed));
} sspT;

void getStatus()
{
    uart0_puts("Getting status...\n");
    vTaskDelay(1000);
    
    sspT status;

    status.byte = LPC_SSP1->SR;

    if (status.byte == 0)
    {
        printf("SSP1 is idle\n");
    }

    else
    {

    printf("status byte: %x\n", status.byte);

    printf("Transmit FIFO empty/full: %x\n", status.tfe);

    printf("transmit fifo full/not full: %x\n", status.tnf);

    printf("Receive FIFO is empty/not empty: %x\n", status.rne);

    printf("Receive FIFO is full/not full: %x\n", status.rff);

    printf("busy/not busy: %x\n", status.bsy);

    }

    // if (LPC_SSP1->SR & (1 << 0))
    // {
    //     uart0_puts("Transmit FIFO is empty\n");
    // }

    // if (LPC_SSP1->SR & (1 << 1))
    // {
    //     uart0_puts("Transmit FIFO is not full\n");
    // }

    // if (LPC_SSP1->SR & (1 << 2))
    // {
    //     uart0_puts("Receive FIFO is not empty\n");
    // }

    // if (LPC_SSP1->SR & (1 << 3))
    // {
    //     uart0_puts("Receive FIFO is full\n");
    // }

    // if (LPC_SSP1->SR & (1 << 4))
    // {
    //     uart0_puts("SSP1 is busy.  It is sending or receiving a frame\n");
    // }

    // else
    // {
    //     uart0_puts("SSP1 is idle\n");
    // }
    //vTaskDelay(1000);

}

class MP3SPI
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
    bool initialize(uint8_t data_size_select, FrameModes format, uint8_t divide)
    {
        if (format < 0 || format > 2)
        {
            return false;
        }
        if (data_size_select > 16 || data_size_select < 4)
        {
            return false;
        }

        if (divide % 2 != 0 || divide == 0)
        {
            return false;
        }

        /* I gots to edit which SPI port I am using for the old MP3 Project */

        
        LPC_SC->PCONP |= (1 << 10);     //enable power for ssp1
        LPC_SC->PCLKSEL0 &= ~(3 << 20);      //select peripheral clock for ssp1
        LPC_SC->PCLKSEL0 |= (2 << 20);  //pcclk = cclck/2

        LPC_PINCON->PINSEL0 &= ~((3 << 14) | (3 << 16) | (3<<18));        //clear the bits
        LPC_PINCON->PINSEL0 |= (2 << 14);        //sclk1 P0.7
        LPC_PINCON->PINSEL0 |= (2 << 16);        //MISO1 P0.8
        LPC_PINCON->PINSEL0 |= (2 << 18);        //MOSI1 P0.9

        LPC_SSP1->CR0 |= (format << 4);
        LPC_SSP1->CR0 = data_size_select-1;	// how many bits are being transferred
        LPC_SSP1->CPSR = divide;         //clock division
        LPC_SSP1->CR1 |= (1<<1);	//enable SSP 
        
        

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
        //getStatus();
        LPC_SSP1->DR = send;
        //getStatus();
        while(LPC_SSP1->SR & (1 << 4));     //SR = status register
        //getStatus();
        //while the ssp is busy
        return LPC_SSP1->DR;        //return the byte
    }

    MP3SPI()
    {

    }

    ~MP3SPI()
    {

    }
  
 private:
  
};

#endif