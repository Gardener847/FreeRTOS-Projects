#include "LCD.h"

LCD::LCD() {
    lct_init()
}

~LCD::LCD(){}

void LCD::writecommand(uint8_t c) {
    LPC_GPIO0->FIOCLR |= (0x1<<21);
    spiwrite(c);
}

void LCD::spiwrite(uint8_t c) {
    int pnum = 0;
    src_addr[0] = c;
    SSP_SSELToggle( pnum, 0 );
    SSPSend( pnum, (uint8_t *)src_addr, 1 );
    SSP_SSELToggle( pnum, 1 );
}


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
bool LCD::lcd_init(uint8_t port_num, uint8_t data_size_select, FrameModes format, uint8_t divide) {
    if (port_num > 1 || port_num < 0)
        return false;
    if (data_size_select < 4 || data_size_select > 16) 
        return false;
    if (format > 2)
        return false;
    if (divide < 2 || (divide % 2) == 1) 
        return false;

    if (port_num == 0) {
        //SSP1 interface power/clock control bit
        LPC_SC->PCONP |= (1 << 21);
        //clear and set peripheral clock for SSP1
        LPC_SC->PCLKSEL1 &= ~(3 << 10);
        LPC_SC->PCLKSEL1 |= (1 << 10);
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
    }
    else if (port_num == 1) {
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
    }

    return true;
}

/**
 * Transfers a byte via SSP to an external device using the SSP data register.
 * This region must be protected by a mutex static to this class.
 *
 * @return received byte from external device via SSP data register.
 */
uint8_t LCD::draw_or_type(uint8_t send){
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