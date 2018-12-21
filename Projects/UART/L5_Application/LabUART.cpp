#include <stdint.h>
#include "LabUART.h"
#include "printf_lib.h"
#include "LPC17xx.h"
#include "uart2.hpp"
#include "io.hpp"
#include "eint.h"

uint8_t num, val;

LabUART::LabUART(){}

LabUART::~LabUART(){}

//receive data from connected board
uint8_t LabUART::receive(void) {
    if (num == 0) {
        //overrun error
        if (LPC_UART0->LSR & (1 << 1)) {
            u0_dbg_printf("Overrun error: recent byte received before previous byte was read by CPU\n");
        }
        //parity error
        if (LPC_UART0->LSR & (1 << 2)) {
            u0_dbg_printf("Parity error\n");
        }
        //framing error (stop bit is 0, when it's supposed to be 1)
        if (LPC_UART0->LSR & (1 << 3)) {
            u0_dbg_printf("Framing error\n");
        }
        LPC_UART0->LCR &= ~(1 << 7);
        val = LPC_UART0->RBR;
        if (val > 100) {
            u0_dbg_printf("Got data: -%d\n", (255 - val + 1));
        }
        else {
            u0_dbg_printf("Got data: %d\n", val);
        }
        LPC_UART0->LCR &= ~(1 << 7);
        
        return val;
    }
    else if (num == 2) {
        //overrun error
        if (LPC_UART2->LSR & (1 << 1)) {
            u0_dbg_printf("Overrun error: recent byte received before previous byte was read by CPU\n");
        }
        //parity error
        if (LPC_UART2->LSR & (1 << 2)) {
            u0_dbg_printf("Parity error\n");
        }
        //framing error (stop bit is 0, when it's supposed to be 1)
        if (LPC_UART2->LSR & (1 << 3)) {
            u0_dbg_printf("Framing error\n");
        }
        LPC_UART2->LCR &= ~(1 << 7);
        val = LPC_UART2->RBR;
        if (val > 100) {
            u0_dbg_printf("Got data: -%d\n", (255 - val + 1));
        }
        else {
            u0_dbg_printf("Got data: %d\n", val);
        }
        LPC_UART2->LCR &= ~(1 << 7);
        
        return val;
    }
    else if (num == 3) {
        //overrun error
        if (LPC_UART3->LSR & (1 << 1)) {
            u0_dbg_printf("Overrun error: recent byte received before previous byte was read by CPU\n");
        }
        //parity error
        if (LPC_UART3->LSR & (1 << 2)) {
            u0_dbg_printf("Parity error\n");
        }
        //framing error (stop bit is 0, when it's supposed to be 1)
        if (LPC_UART3->LSR & (1 << 3)) {
            u0_dbg_printf("Framing error\n");
        }
        LPC_UART3->LCR &= ~(1 << 7);
        val = LPC_UART3->RBR;
        if (val > 100) {
            u0_dbg_printf("Got data: -%d\n", (255 - val + 1));
        }
        else {
            u0_dbg_printf("Got data: %d\n", val);
        }
        LPC_UART3->LCR &= ~(1 << 7);
        
        return val;
    }
}

//send data passed by client
void LabUART::transmit(uint8_t c) {
    uint32_t neg = 0;
    if (num == 0) {
        //divisor latch access bit (DLAB) in LCR must be zero to access the UnTHR (transmit holding register)
		LPC_UART0->LCR &= ~(1 << 7);
        //store data in transmit holding register
        LPC_UART0->THR = c;
        //print in 7-seg
        if (c > 100) {
            neg = 255 - c + 1;
            LD.setLeftDigit('-');
            u0_dbg_printf("Sending data -%d...\n", neg);
        }
        else {
            LD.setNumber(c);
            u0_dbg_printf("Sending data %d...\n", c);
        }
        if ((LPC_UART0->LSR & (1 << 6)) == 0) {
            u0_dbg_printf("UnTHR and/or UnTSR contains valid data\n");
        }
        //transmit enable register (TER) will continue to send data
		//as long as they're available, or until register is set to 0
		LPC_UART0->TER |= (1 << 7);
    }
    else if (num == 2) {
        //divisor latch access bit (DLAB) in LCR must be zero to access the UnTHR (transmit holding register)
		LPC_UART2->LCR &= ~(1 << 7);
        //store data in transmit holding register
        LPC_UART2->THR = c;
        //print in 7-seg
        if (c > 100) {
            neg = 255 - c + 1;
            LD.setLeftDigit('-');
            u0_dbg_printf("Sending data -%d...\n", neg);
        }
        else {
            LD.setNumber(c);
            u0_dbg_printf("Sending data %d...\n", c);
        }
        if ((LPC_UART2->LSR & (1 << 6)) == 0) {
            u0_dbg_printf("UnTHR and/or UnTSR contains valid data\n");
        }
        //transmit enable register (TER) will continue to send data
		//as long as they're available, or until register is set to 0
		LPC_UART2->TER |= (1 << 7);
    }
    else if (num == 3) {
        LPC_UART3->LCR &= ~(1 << 7);
        LPC_UART3->THR = c;
        if (c > 100) {
            neg = 255 - c + 1;
            LD.setLeftDigit('-');
            u0_dbg_printf("Sending data -%d...\n", neg);
        }
        else {
            LD.setNumber(c);
            u0_dbg_printf("Sending data %d...\n", c);
        }
        u0_dbg_printf("Sending data %d...\n", c);
        if ((LPC_UART3->LSR & (1 << 6)) == 0) {
            u0_dbg_printf("UnTHR and/or UnTSR contains valid data\n");
        }
        LPC_UART3->TER |= (1 << 7);
    }
}

//initialize and configure pins for UART2/3
void LabUART::initialize(UARTn uartN) {
    if (uartN == UART0) {
        num = 0;

        LPC_SC->PCONP |= (1 << 3);
        LPC_SC->PCLKSEL0 &= ~(3 << 6);
        LPC_SC->PCLKSEL0 |= (1 << 6);
        //clear and pints for TX and RX respectively
        LPC_PINCON->PINSEL4 &= ~(3 << 4);
        LPC_PINCON->PINSEL4 |= (2 << 4);
        LPC_PINCON->PINSEL4 &= ~(3 << 6);
        LPC_PINCON->PINSEL4 |= (2 << 6);

        //set baud rate to 9600bps
        uint16_t div = sys_get_cpu_clock() / (16 * 9600);	
        LPC_UART0->DLL = (div >> 0) & 0xff;
        LPC_UART0->DLM = (div >> 8) & 0xff;

        //set word length to 8bits
        LPC_UART0->LCR |= (3 << 0);
        //only use 1 stop bit
        LPC_UART0->LCR &= ~(1 << 2);
        //enable parity generation and checking b.c of noise
        LPC_UART0->LCR |= (1 << 3);
        //set parity bit to even
        LPC_UART0->LCR &= ~(3 << 4);
        LPC_UART0->LCR |= (1 << 4);
        //disable break transmission
        LPC_UART0->LCR &= ~(1 << 6);
        //set DLAB to change baud rate
        LPC_UART0->LCR |= (1 << 7);

        //set FIFO, and reset RX and TX FIFO
        LPC_UART0->FCR = (7 << 0);
        //Clear DLAB after setting DLL, DLM and to enable interrupt
        LPC_UART0->LCR  &= ~(1 << 7);
        //enable receive data available interrupt and disable others
        LPC_UART0->IER = 0x1;
    }
    else if (uartN == UART2) {
        num = 2;

        LPC_SC->PCONP |= (1 << 24);
        LPC_SC->PCLKSEL1 &= ~(3 << 16);
        LPC_SC->PCLKSEL1 |= (1 << 16);
        //clear and pints for TX and RX respectively
        LPC_PINCON->PINSEL4 &= ~(3 << 16);
        LPC_PINCON->PINSEL4 |= (2 << 16);
        LPC_PINCON->PINSEL4 &= ~(3 << 18);
        LPC_PINCON->PINSEL4 |= (2 << 18);

        //set baud rate to 9600bps
        uint16_t div = sys_get_cpu_clock() / (16 * 9600);	
        LPC_UART2->DLL = (div >> 0) & 0xff;
        LPC_UART2->DLM = (div >> 8) & 0xff;

        //set word length to 8bits
        LPC_UART2->LCR |= (3 << 0);
        //only use 1 stop bit
        LPC_UART2->LCR &= ~(1 << 2);
        //enable parity generation and checking b.c of noise
        LPC_UART2->LCR |= (1 << 3);
        //set parity bit to even
        LPC_UART2->LCR &= ~(3 << 4);
        LPC_UART2->LCR |= (1 << 4);
        //disable break transmission
        LPC_UART2->LCR &= ~(1 << 6);
        //set DLAB to change baud rate
        LPC_UART2->LCR |= (1 << 7);

        //set FIFO, and reset RX and TX FIFO
        LPC_UART2->FCR = (7 << 0);
        //Clear DLAB after setting DLL, DLM and to enable interrupt
        LPC_UART2->LCR  &= ~(1 << 7);
        //enable receive data available interrupt and disable others
        LPC_UART2->IER = 0x1;
    }
    else if (uartN == UART3) {
        num = 3;

        LPC_SC->PCONP |= (1 << 25);
        LPC_SC->PCLKSEL1 &= ~(3 << 18);
        LPC_SC->PCLKSEL1 |= (1 << 18);
        LPC_PINCON->PINSEL0 &= ~(3 << 0);
        LPC_PINCON->PINSEL0 |= (2 << 0);
        LPC_PINCON->PINSEL0 &= ~(3 << 2);
        LPC_PINCON->PINSEL0 |= (2 << 2);

        //set baud rate to 9600bps
        uint16_t div = sys_get_cpu_clock() / (16 * 9600);	
        LPC_UART3->DLL = (div >> 0) & 0xff;
        LPC_UART3->DLM = (div >> 8) & 0xff;

        //set word length to 8bits
        LPC_UART3->LCR |= (3 << 0);
        //only use 1 stop bit
        LPC_UART3->LCR &= ~(1 << 2);
        //enable parity generation and checking b.c of noise
        LPC_UART3->LCR |= (1 << 3);
        //set parity bit to even
        LPC_UART3->LCR &= ~(3 << 4);
        LPC_UART3->LCR |= (1 << 4);
        //disable break transmission
        LPC_UART3->LCR &= ~(1 << 6);
        //set DLAB to change baud rate
        LPC_UART3->LCR |= (1 << 7);

        //set FIFO, and reset RX and TX FIFO
        LPC_UART3->FCR = (7 << 0);
        //Clear DLAB after setting DLL, DLM and to enable interrupt
        LPC_UART3->LCR  &= ~(1 << 7);
        //enable receive data available interrupt and disable others
        LPC_UART3->IER = 0x1;
    }
}