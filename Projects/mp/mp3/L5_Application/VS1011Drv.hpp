#ifndef VS1011DRV_H
#define VS1011DRV_H
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
#include "MP3GPIO.hpp"
#include "labSPI.hpp"
#include "printf_lib.h"
//#include "ADCDrv.hpp"
#include "spi_sem.h"
/*
    This Drv should do the following
    -Setup and maintain the SPI communication with the 
*/
enum MP3State
{
    PAUSED,
    PLAYING,
    STOPPED
};
struct PlayBackControl
{
    MP3State currentState;
    PlayBackControl() : currentState(STOPPED) {}
    MP3State getCurrentState()
    {
        return currentState;
    }
};
class VS1011Drv
{
    public:
    static VS1011Drv * instance();
    uint8_t  m_transBuf[512];

    //This is the queue handle that the flash read Task will fill
    QueueHandle_t * m_transmitQueue;

    //should set up the SPI and init the VS1011
    //Start the tasks for sending data to the decoder
    //set queue handler
    void initialize();

    //this function is purely for testing purposes, it sends a series of commands that initiate the sine test in the decoder
    //a sine wave will be output as audio, will simply test that the decoder is working.
    void SineTestStart();

    void SineTestStop();

    //#######################################TASKS#############################################
    //this task will wait on the queue being filled by the flash reader
    //it will take the first item off the queue and send it to the decoder
    void SendToDecoder();

    void monitorVolume();

    bool stopSong();

    void HWreset();
    //This is used for initial setup, sending config commands to the flash
    void SendSCIWriteCommand(uint8_t regAddr, uint16_t writeData);

    uint16_t SendSCIReadCommand(uint8_t regAddr);

    void setVolume(uint16_t db);

    //sets the member variable queue handler to the one created by main
    void setQueueHandler(QueueHandle_t &flashQueue);

    void selectDCS();

    void deselectDCS();

    void selectxDCS();

    void deselectxDCS();
    PlayBackControl * itsPlayBackControl;
    private:
    static VS1011Drv * m_instance;
    LabSPI * m_SPIDrv;

    
    //ADCDrv m_ADCDrv;
    //chip select GPIO pins
    //one for SCI commands, the other for SDI data
    MP3GPIO * xDCS;
    MP3GPIO * DCS;
    //this is the decoder output that lets us know when we can send data.
    MP3GPIO * DREQ;
    MP3GPIO * RST;
    VS1011Drv();
    ~VS1011Drv();
};

#endif