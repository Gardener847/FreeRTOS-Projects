#pragma once
#ifndef SPI_H
#define SPI_H

class SPI{
    public:
        SPI();
        ~SPI();
        bool spit_init(uint8_t portNum, uint8_t pinNum);
        bool spi_write(uint8_t portNum, uint8_t *buf);
        void adesto_ds(uint8_t pinNum);
        void adesto_cs(uint8_t pinNum);
};
#endif