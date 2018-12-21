#include "SPI.h"

SPI::SPI(){}

SPI::~SPI(){}

bool SPI::spit_init(uint8_t portNum, uint8_t pinNum){
    if (portNum > 1 || portNum < 0){
        return false;
    }
    else if (pinNum < 0 || pinNum > 31) {
        return false;
    }

    if (portNum == 0) {
        //SSP1 interface power/clock control bit
        LPC_SC->PCONP |= (1 << 21);
        //clear and set peripheral clock for SSP1
        LPC_SC->PCLKSEL1 &= ~(3 << 10);
        LPC_SC->PCLKSEL1 |= (1 << 10);
        //P0.6 is used as a GPIO output and acts as a Slave select for flash
        LPC_GPIO0->FIODIR |= (1 << pinNum);
        LPC_GPIO0->FIOSET = (1 << pinNum);
        //clear and set pins for SCK0, SSEL0, MISO0, and MOSI0
        LPC_PINCON->PINSEL0 &= ~(3 << 30);
        LPC_PINCON->PINSEL0 |= (2 << 30);
        LPC_PINCON->PINSEL1 &= ~((3 << 0) | (3 << 2) | (3 << 4));
        LPC_PINCON->PINSEL1 |= ((2 << 0) | (2 << 2) | (2 << 4));

        // Set data size (bit 3:0); frame format (bit 5:4); clock polarity to start low value on SCK pin (bit 6); clock phase to trigger on first edge (bit 7)
        // Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15
        LPC_SSP0->CR0 = 0x0707;
        // CPSDVSR is the clock prescale divisor and the APB clock PCLK clocks the prescaler, the bit frequency is PCLK / (CPSDVSR × [SCR+1]).
        // SSPCPSR clock prescale register, master mode, minimum divisor is 0x02
        LPC_SSP0->CPSR = 0x2;
        // set loop back mode to 0 (bit 0); SSP Enable (bit 1); set controller as master (bit 2)
        LPC_SSP0->CR1 = 0x2;

        // clear RxFIFO
        uint8_t i, dummy;
        for (i = 0; i < 8; i++) {
            dummy = LPC_SSP0->DR;
        }
    }
    else if (portNum == 1) {
        //SSP1 interface power/clock control bit
        LPC_SC->PCONP |= (1 << 10);
        //clear and set peripheral clock for SSP1
        LPC_SC->PCLKSEL0 &= ~(3 << 20);
        LPC_SC->PCLKSEL0 |= (1 << 20);
        //P0.6 is used as a GPIO output and acts as a Slave select for flash
        LPC_GPIO0->FIODIR |= (1 << pinNum);
        LPC_GPIO0->FIOSET = (1 << pinNum);
        //clear and set pins for SCK0, SSEL1, MISO1, and MOSI1
        LPC_PINCON->PINSEL0 &= ~((3 << 14) | (3 << 16) | (3 << 18));
        LPC_PINCON->PINSEL0 |= ((2 << 14) | (2 << 16) | (2 << 18));

        // Set data size (bit 3:0); frame format (bit 5:4); clock polarity to start low value on SCK pin (bit 6); clock phase to trigger on first edge (bit 7)
        // Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15
        LPC_SSP1->CR0 = 0x0707;
        // CPSDVSR is the clock prescale divisor and the APB clock PCLK clocks the prescaler, the bit frequency is PCLK / (CPSDVSR × [SCR+1]).
        // SSPCPSR clock prescale register, master mode, minimum divisor is 0x02
        LPC_SSP1->CPSR = 0x2;
        // set loop back mode to 0 (bit 0); SSP Enable (bit 1); set controller as master (bit 2)
        LPC_SSP1->CR1 = 0x2;

        // clear RxFIFO
        uint8_t i, dummy;
        for (i = 0; i < 8; i++) {
            dummy = LPC_SSP1->DR;
        }
    }

    return true;
}

bool SPI::spi_write(uint8_t portNum, uint8_t *buf){
    if (portNum == 0) {
        /* Move on only if NOT busy and TX FIFO not full. */
        while ((LPC_SSP0->SR & 0x12) != (1 << 1));
        LPC_SSP0->DR = *buf;
        buf++;
        //while busy (bit 4) in status register wait
        while ((LPC_SSP0->SR & (1 << 4)));
        return LPC_SSP0->DR;
    }
    else if (portNum == 1) {
        /* Move on only if NOT busy and TX FIFO not full. */
        while ((LPC_SSP1->SR & 0x12) != (1 << 1));
        LPC_SSP1->DR = *buf;
        buf++;
        //while busy (bit 4) in status register wait
        while ((LPC_SSP1->SR & (1 << 4)));
        return LPC_SSP1->DR;
    }
}

void SPI::adesto_ds(uint8_t pinNum){
    //asserts chip select by setting signal low
	LPC_GPIO0->FIOCLR = (1 << pinNum);
}

void SPI::adesto_cs(uint8_t pinNum){
    //de-asserts chips elect by setting signal high
	LPC_GPIO0->FIOSET = (1 << pinNum);
}