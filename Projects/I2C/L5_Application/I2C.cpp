#include <stddef.h>
#include <stdint.h>
#include <i2c_base.hpp>
#include "printf_lib.h"
#include "eint.h"
#include "FreeRTOS.h"
#include "I2C.h"
#include "io.hpp"
#include "LPC17xx.h"

uint8_t readBuff[256];
uint32_t someIn = 0;
uint32_t buffIn = 0;
bool checker = false;

SlaveI2C::SlaveI2C(){}

SlaveI2C::~SlaveI2C(){}

//19.95 table 400 & 401
void SlaveI2C::slaveISR(void) {
    u0_dbg_printf("Inside isr\n");
    buffIn = 1;

    //receive mode
    //no I2DAT and no STDAT action
    if ((LPC_I2C2->I2STAT == 0x60) || (LPC_I2C2->I2STAT == 0x68) || (LPC_I2C2->I2STAT == 0x70) 
        || (LPC_I2C2->I2STAT == 0x78) || (LPC_I2C2->I2STAT == 0x88) || (LPC_I2C2->I2STAT == 0x98)) {
        LPC_I2C2->I2CONCLR = 0x8;
        LPC_I2C2->I2CONSET = 0x4;
        buffIn = 0;

        if (LPC_I2C2->I2STAT == 0x60) {
            u0_dbg_printf("case 0x60\n");
        }
        else if (LPC_I2C2->I2STAT == 0x68) {
            u0_dbg_printf("case 0x68\n");
        }
        else if (LPC_I2C2->I2STAT == 0x70) {
            u0_dbg_printf("case 0x70\n");
        }
        else if (LPC_I2C2->I2STAT == 0x78) {
           u0_dbg_printf("case 0x78\n");
        }
        else if (LPC_I2C2->I2STAT == 0x88) {
           u0_dbg_printf("case 0x88\n");
        }
        else if (LPC_I2C2->I2STAT == 0x98) {
           u0_dbg_printf("case 0x98\n");
        }
        else if (LPC_I2C2->I2STAT == 0xA0) {
           u0_dbg_printf("case 0xA0\n");
           checker = false;
        }
        return;
    }
    else if (LPC_I2C2->I2STAT == 0x80) {
        u0_dbg_printf("case 0x80\n");
        if(checker == 0) {
            //read and print out received data
            buffIn = LPC_I2C2->I2DAT;
            u0_dbg_printf("index: %d\n", buffIn);
            checker = true;
        }
        else {
            readBuff[buffIn] = LPC_I2C2->I2DAT ;
            u0_dbg_printf("value: %x\n", readBuff[buffIn]);
        }
        if(buffIn == 0) {
            LPC_I2C2->I2CONCLR = 0xC;
        }
        else {
            LPC_I2C2->I2CONCLR = 0x8;
            LPC_I2C2->I2CONSET = 0x4;
        }
        return;
    }
    else if (LPC_I2C2->I2STAT == 0x90) {
        u0_dbg_printf("case 0x90\n");
        u0_dbg_printf("buffer: %x\n", readBuff[buffIn]);
        readBuff[buffIn] = LPC_I2C2->I2DAT;
        LPC_I2C2->I2CONCLR = 0xC;
        return;
    }

    //transmit mode
    if ((LPC_I2C2->I2STAT == 0xA8) || (LPC_I2C2->I2STAT == 0xB0) || (LPC_I2C2->I2STAT == 0xB8) 
        || (LPC_I2C2->I2STAT == 0xC8) || (LPC_I2C2->I2STAT == 0xC8)) {
        LPC_I2C2->I2CONCLR = 0x8;
        LPC_I2C2->I2CONSET = 0x4;
        if (LPC_I2C2->I2STAT == 0xA8) {
            u0_dbg_printf("case 0xA8\n");
            u0_dbg_printf("buffer: %x\n", readBuff[buffIn]);
            //load data
            LPC_I2C2->I2DAT = readBuff[buffIn];
        }
        else if (LPC_I2C2->I2STAT == 0xB0) {
            u0_dbg_printf("case 0xB0\n");
            //load data
            LPC_I2C2->I2DAT = readBuff[buffIn];
        }
        //data transmitted and ACK was received, load next data in next index
        else if (LPC_I2C2->I2STAT == 0xB8) {
            u0_dbg_printf("case 0xB8\n");
            LPC_I2C2->I2DAT = readBuff[someIn++];
        }
        else if (LPC_I2C2->I2STAT == 0xC0) {
            u0_dbg_printf("case 0xC0\n");
        }
        else if (LPC_I2C2->I2STAT == 0xC8) {
            u0_dbg_printf("case 0xC8\n");
        }
        return;
    }
}

bool SlaveI2C::init (uint8_t addr) {
    //power up I2C interface 2 power/control bit
    LPC_SC->PCONP |= (1 << 26);
    //set I2C peripheral clock to CCLK
    LPC_SC->PCLKSEL1 |= (1 << 20);

    //clear and set SDA2 and SCL2
    LPC_PINCON->PINSEL0 &= ~(3 << 20);
    LPC_PINCON->PINSEL0 |= (2 << 20);
    LPC_PINCON->PINSEL0 &= ~(3 << 22);
    LPC_PINCON->PINSEL0 |= (2 << 22);

    //set to neither pull-up nor pull-down for SDA2 and SCL2 respectively
    LPC_PINCON->PINMODE0 &= ~(3 << 20);
    LPC_PINCON->PINMODE0 |= (2 << 20);
    LPC_PINCON->PINMODE0 &= ~(3 << 22);
    LPC_PINCON->PINMODE0 |= (2 << 22);
    //open drain mode for P0.10 and 11
    LPC_PINCON->PINMODE_OD0 |= (3 << 10);

    //set I2C bit frequency to standard 100KHz
    LPC_I2C2->I2SCLH = 900;
    LPC_I2C2->I2SCLL = 900;

    //clear and set slave addresses for I2C interface communication

    // LPC_I2C2->I2ADR1 = 0;
    // LPC_I2C2->I2ADR2 = 0;
    // LPC_I2C2->I2ADR3 = 0;
    LPC_I2C2->I2ADR0 = addr;
    // LPC_I2C2->I2ADR1 = addr + 1;
    // LPC_I2C2->I2ADR2 = addr + 2;
    // LPC_I2C2->I2ADR3 = addr + 3;

    LPC_I2C2->I2MASK0 = addr;
    // LPC_I2C2->I2MASK1 = 0;
    // LPC_I2C2->I2MASK2 = 0;
    // LPC_I2C2->I2MASK3 = 0;
    // LPC_I2C2->I2MASK1 = addr + 1;
    // LPC_I2C2->I2MASK2 = addr + 2;
    // LPC_I2C2->I2MASK3 = addr + 3;

    LPC_I2C2->I2DAT = addr;

    //set I2EN and AA bits, enabling slave functions
    LPC_I2C2->I2CONCLR = 0xff;
    // LPC_I2C2->I2CONSET |= (1 << 2);
    LPC_I2C2->I2CONSET |= (1 << 6);

    //setup isr handler when reading data from slave
    isr_register(I2C2_IRQn, slaveISR);
    NVIC_EnableIRQ(I2C2_IRQn);

    return true;
}