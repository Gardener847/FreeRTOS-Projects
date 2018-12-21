#pragma once
#ifndef LABSPI_H
#define LABSPI_H
#include <stdint.h>

class LabSpi
{
 public:
    enum FrameModes{
        spi = 0,
        ti = 1,
        microwire = 2
    };

    typedef union{
	    uint8_t byte1;
        struct 
        {
            uint8_t pageConfig: 1;
            uint8_t sectorStatus: 1;
            uint8_t densityCode: 4;
            uint8_t comprReslt: 1;
            uint8_t rdyBsyStat1_1: 1;
        } __attribute__((packed));

        uint8_t byte2;
        struct 
        {
            uint8_t erase: 1;
            uint8_t buff1Stat: 1;
            uint8_t buff2Stat: 1;
            uint8_t sectorLock: 1;
            uint8_t rsrv1: 1;
            uint8_t ersProgError: 1;
            uint8_t rsrv2: 1;
            uint8_t rdyBsyStat2_1: 1;
        } __attribute__((packed));
    } adlx_t;

    typedef union {
        uint8_t buffer[512];
        struct {
            uint8_t daemon [446];
            uint8_t part1 [16];
            uint8_t part2 [16];
            uint8_t part3 [16];
            uint8_t part4 [16];
            uint8_t bootSig [4];
        } __attribute__((packed));
    } gMBR_t;

    bool initialize(uint8_t data_size_select, FrameModes format, uint8_t divide);
    uint8_t transfer(uint8_t send);
    void adestoCS(void);
    void adestoDS(void);
    void readFlashStatus(void);
    void readSig(void);

    LabSpi();
    ~LabSpi();
};

#endif