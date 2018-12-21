#pragma once
#ifndef LABUART_H
#define LABUART_H

#include "LPC17xx.h"
#include <stdint.h>

class LabUART{
public:
    enum UARTn
    {
        UART0,
        UART2,
        UART3
    };

    LabUART();
    ~LabUART();
    void initialize(UARTn uartN);
    uint8_t receive(void);
    void transmit(uint8_t c);
    static void my_uart_rx_intr(void);
};
#endif