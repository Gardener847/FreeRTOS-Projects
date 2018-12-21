#include "printf_lib.h"
#include "LPC17xx.h"
#include "LabSpi.h"

LabSpi::LabSpi(){}
LabSpi::~LabSpi(){}

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
bool LabSpi::initialize(uint8_t data_size_select, FrameModes format, uint8_t divide){
    if (data_size_select < 4 || data_size_select > 16) 
        return false;
    if (format > 2)
        return false;
    if (divide < 2 || (divide % 2) == 1) 
        return false;

    //SSP1 interface power/clock control bit
	LPC_SC->PCONP |= (1 << 10);
	//clear and set peripheral clock for SSP1
	LPC_SC->PCLKSEL0 &= ~(3 << 20);
	LPC_SC->PCLKSEL0 |= (1 << 20);
	//P0.6 is used as a GPIO output and acts as a Slave select for flash
	LPC_GPIO0->FIODIR |= (1 << 6);
	LPC_GPIO0->FIOSET = (1 << 6);
	//clear and set pins for SCK0, SSEL1, MISO1, and MOSI1
	LPC_PINCON->PINSEL0 &= ~((3 << 14) | (3 << 16) | (3 << 18));
    LPC_PINCON->PINSEL0 |= ((2 << 14) | (2 << 16) | (2 << 18));

	// Set data size (bit 3:0); frame format (bit 5:4); clock polarity to start low value on SCK pin (bit 6); clock phase to trigger on first edge (bit 7)
    LPC_SSP1->CR0 = 0x0;
    LPC_SSP1->CR0 = data_size_select;
    if (format == spi) 
        LPC_SSP1->CR0 |= (0 << 4);
    else if (format == ti)
        LPC_SSP1->CR0 |= (1 << 4);
    else if (format == microwire)
        LPC_SSP1->CR0 |= (2 << 4);
	// set loop back mode to 0 (bit 0); SSP Enable (bit 1); set controller as master (bit 2)
	LPC_SSP1->CR1 = 0x2;
	// CPSDVSR is the clock prescale divisor and the APB clock PCLK clocks the prescaler, the bit frequency is PCLK / (CPSDVSR × [SCR+1]).
	LPC_SSP1->CPSR = divide;

    return true;
}

/**
 * Transfers a byte via SSP to an external device using the SSP data register.
 * This region must be protected by a mutex static to this class.
 *
 * @return received byte from external device via SSP data register.
 */
uint8_t LabSpi::transfer(uint8_t send){
    LPC_SSP1->DR = send;
	//while busy (bit 4) in status register wait
	while ((LPC_SSP1->SR & (1 << 4)));
	return LPC_SSP1-> DR;
}

void LabSpi::adestoCS(void) {
    //asserts chip select by setting signal low
	LPC_GPIO0->FIOCLR = (1 << 6);
    LPC_GPIO0->FIOCLR = (1 << 28);
}
    
void LabSpi::adestoDS(void) {
    //de-asserts chips elect by setting signal high
	LPC_GPIO0->FIOSET = (1 << 6);
    LPC_GPIO0->FIOSET = (1 << 28);
}

void LabSpi::readFlashStatus(void){
    transfer(0xD7);
    adlx_t status;
    status.byte1 = transfer(0xD7);
    status.byte2 = transfer(0xD7);
    u0_dbg_printf("\nByte 1 Status\n");
    u0_dbg_printf("--------------------\n");
    // if (stat2 & 0x1)
    status.pageConfig ?
        u0_dbg_printf("Device is configured for 'power of 2' binary page size (512 bytes): \t %x\n", status.pageConfig)
        :
        u0_dbg_printf("Device is configured for standard DataFlash page size (528 bytes): \t %x\n", status.pageConfig);
    // if ((stat2 & 0x2) >> 1) 
    status.sectorStatus ?
        u0_dbg_printf("Sector protection is enabled: \t\t\t\t\t\t %x\n", status.sectorStatus)
        :
        u0_dbg_printf("Sector protection is disabled: \t\t\t\t\t\t %x\n", status.sectorStatus);
    // ((stat2 & 0x3C) >> 2)
    u0_dbg_printf("Density Code 16-Mbit: \t\t\t\t\t\t\t %x\n", status.densityCode);
    // if ((stat2 & 0x40) >> 6) 
    status.comprReslt ?
        u0_dbg_printf("Main memory page data doesn't match buffer data: \t\t\t %x\n", status.comprReslt)
        :
        u0_dbg_printf("Main memory page data matches buffer data: \t\t\t\t %x\n", status.comprReslt);
    // if ((stat2 & 0x80) >> 7) 
    status.rdyBsyStat1_1 ?
        u0_dbg_printf("Device is ready: \t\t\t\t\t\t\t %x\n", status.rdyBsyStat1_1)
        :
        u0_dbg_printf("Device is busy with an internal operation: \t %x\n", status.rdyBsyStat1_1);
    
    u0_dbg_printf("\nByte 2 Status\n");
    u0_dbg_printf("--------------------\n");
    // if (stat3 & 0x1)
    status.erase ?
        u0_dbg_printf("A sector is erase suspended: \t\t\t\t %x\n", status.erase)
        :
        u0_dbg_printf("No sectors are erase suspended: \t\t\t\t %x\n", status.erase);
    // if ((stat3 & 0x2) >> 1)
    status.buff1Stat ?
        u0_dbg_printf("A sector is program suspended while using Buffer 1: \t %x\n", status.buff1Stat)
        :
        u0_dbg_printf("No program operation has been suspended while using Buffer 1: \t %x\n", status.buff1Stat);
    // if ((stat3 & 0x4) >> 2)
    status.buff2Stat ?
        u0_dbg_printf("A sector is program suspended while using Buffer 2: \t %x\n", status.buff2Stat)
        :
        u0_dbg_printf("No program operation has been suspended while using Buffer 2: \t %x\n", status.buff2Stat);
    // if ((stat3 & 0x8) >> 3)
    status.sectorLock ?
        u0_dbg_printf("Sector Lockdown command is enabled: \t\t\t\t %x\n", status.sectorLock)
        :
        u0_dbg_printf("Sector Lockdown command is disabled: \t\t\t\t %x\n", status.sectorLock);
    // ((stat3 & 0x10) >> 4)
    u0_dbg_printf("Reserved for Future Use: \t\t\t\t\t %x\n", status.rsrv1);
    // if ((stat3 & 0x20) >> 5)
    status.ersProgError ?
        u0_dbg_printf("Erase or program error detected: \t\t\t\t %x\n", status.ersProgError)
        :
        u0_dbg_printf("Erase or program operation was successful: \t\t\t %x\n", status.ersProgError);
    // ((stat3 & 0x40) >> 6)
    u0_dbg_printf("Reserved for Future Use: \t\t\t\t\t %x\n", status.rsrv2);
    // if ((stat3 & 0x80) >> 7)
    status.rdyBsyStat2_1 ?
        u0_dbg_printf("Device is ready: \t\t\t\t\t\t %x\n", status.rdyBsyStat2_1)
        :
        u0_dbg_printf("Device is busy with an internal operation: \t\t %x\n", status.rdyBsyStat2_1);
}

void LabSpi::readSig(void)
{
    // Manufacture and Device ID read
    transfer(0x9F);
    u0_dbg_printf("\nDevice Signature\n");
    u0_dbg_printf("------------------\n");
    u0_dbg_printf("\nManufacture ID: \t %x\n", transfer(0x9F));
    u0_dbg_printf("Device ID 1st 8 bytes: \t %x\n", transfer(0x00));
    u0_dbg_printf("Device ID 2nd 8 bytes: \t %x\n", transfer(0x00));
}