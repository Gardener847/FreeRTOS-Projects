#include "VS1011Drv.hpp"

/* TO DO
    -End a song correctly
    -Play multiple songs
    -Stop and play next song
*/
uint8_t const SCI_MODE_REG = 0x0;
uint8_t const SCI_CLOCKF_REG = 0x3;
uint8_t const SCI_VOL_REG = 0xB;
uint8_t const SCI_AUDATA_REG = 0x5;
uint16_t const SCI_WRAM = 0x6;
uint16_t const SCI_WRAMADDR = 0x7;
uint16_t const EDNFB_ADDR = 0x1e06;

VS1011Drv::VS1011Drv()
{

}
VS1011Drv * VS1011Drv::m_instance = NULL;

VS1011Drv * VS1011Drv::instance()
{
    if(!m_instance)
    {
        m_instance = new VS1011Drv();
    }
    return m_instance;
}

//should set up the SPI and init the VS1011
//Start the tasks for sending data to the decoder
//set queue handler
void VS1011Drv::initialize()
{
    //initialize the SPI
    m_SPIDrv = new LabSPI();
    u0_dbg_printf("here1\n");
    m_SPIDrv->initializeDecoderSPI();

    RST = new MP3GPIO;
    RST->initialize(19,1);
    DREQ = new MP3GPIO;
    DREQ->initialize(28,1);
    DCS = new MP3GPIO;
    DCS->initialize(20,1);
    xDCS = new MP3GPIO;
    xDCS->initialize(23,1);
    DREQ->setAsInput();
    DCS->setAsOutput();
    xDCS->setAsOutput();
    RST->setAsOutput();
    
    //HW RESET
    spi1_lock();
    HWreset();

    //SW RESET
    delay_ms(100);
    SendSCIWriteCommand(0x00, 0x0800 | 0x0004);
    delay_ms(100);
    while(!DREQ->getLevel());

    SendSCIWriteCommand(SCI_MODE_REG, 0x0800); //SendSCIWriteCommand(SCI_MODE_REG, 0x0800 | 0x04); //send reset
    SendSCIWriteCommand(SCI_AUDATA_REG, 0xAC45);    //44100Hz stereo
    SendSCIWriteCommand(SCI_CLOCKF_REG, 0x2000);   //CLKI internal clock is now 24.3 MHz
    SendSCIWriteCommand(SCI_VOL_REG, 0x0404);  //set to volume slightly under max, left and right channel put at -2.0 db, so send (4*256) + 4 = 1028 (data sheet black magic calculations) 

    uint16_t result = SendSCIReadCommand(0x00);
    //printf("SCI_MODE REG: %x \n",result);

    result = SendSCIReadCommand(0xB);
    //printf("VOL REG: %x \n",result);

    result = SendSCIReadCommand(0x5);
    //printf("AUData: %x \n",result);

    //send atleast two 0 bytes to the decoder
    deselectxDCS();
    selectDCS();
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    deselectDCS();
    
    SineTestStart();
    delay_ms(2000);
    SineTestStop();
    spi1_unlock();
    //xTaskCreate(monitorVolume, "vMonitorVolume", 1024, NULL, 1, NULL);
}

void VS1011Drv::HWreset()
{
    RST->setLow();
    delay_ms(1000);
    RST->setHigh();
    while(!DREQ->getLevel());
    
    DCS->setHigh();
    xDCS->setHigh(); 
    while(!DREQ->getLevel());

    //place decoder into MP3 mode on hw reset
    SendSCIWriteCommand(SCI_WRAMADDR, 0xC017);
    SendSCIWriteCommand(SCI_WRAM, 3);

    // SendSCIWriteCommand(SCI_WRAMADDR, 0xC017);
    // uint8_t result = SendSCIReadCommand(SCI_WRAM);
    // u0_dbg_printf("result:%x \n",result);

    SendSCIWriteCommand(SCI_WRAMADDR, 0xC019);
    SendSCIWriteCommand(SCI_WRAM, 0);

    // result = SendSCIReadCommand(SCI_WRAM);
    // u0_dbg_printf("result:%x \n",result);
}

//this is driven by a command handler. you can use the terminal to execute the start and stop of the test
void VS1011Drv::SineTestStart()
{
    uint8_t const test = 0b00011111;    //test freq of 5000Hz

    HWreset();

    //SW RESET
    delay_ms(100);
    SendSCIWriteCommand(0x00, 0x0800 | 0x0004);
    delay_ms(100);
    while(!DREQ->getLevel());
    //Send test commands
    SendSCIWriteCommand(SCI_AUDATA_REG, 0xAC45);    //44100Hz stereo
    SendSCIWriteCommand(SCI_CLOCKF_REG, 0x2000);   //CLKI internal clock is now 24.3 MHz
    SendSCIWriteCommand(SCI_VOL_REG, 0x0404); 
    SendSCIWriteCommand(SCI_MODE_REG, 0x0800 | 0x0020); //enable SDI tests
    
    deselectxDCS();
    selectDCS();
    m_SPIDrv->transferMP3(0x53);
    m_SPIDrv->transferMP3(0xEF);
    m_SPIDrv->transferMP3(0x6E);
    m_SPIDrv->transferMP3(test);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    deselectDCS();

    while(!DREQ->getLevel());
}

void VS1011Drv::SineTestStop()
{
    selectDCS();
    deselectxDCS();

    m_SPIDrv->transferMP3(0x45);
    m_SPIDrv->transferMP3(0x78);
    m_SPIDrv->transferMP3(0x69);
    m_SPIDrv->transferMP3(0x74);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);
    m_SPIDrv->transferMP3(0x00);

    deselectDCS();

    while(!DREQ->getLevel());    
}

// this task will wait on the queue being filled by the flash reader
// it will take the first item off the queue and send it to the decoder
void VS1011Drv::SendToDecoder()
{
    uint8_t * buf_ptr;
    while(1)
    {
        if(xQueueReceive(*m_transmitQueue, m_transBuf, portMAX_DELAY))
        {
            buf_ptr = m_transBuf;
            while(!(buf_ptr > &m_transBuf[511]) && (itsPlayBackControl->getCurrentState() != STOPPED))    //while the buf_ptr has not exceeded the end of the chunk
            {   
                if(itsPlayBackControl->getCurrentState() == PAUSED)
                {
                    //u0_dbg_printf("Decoder PAUSE\n");
                    vTaskSuspend(NULL);
                }
                spi1_lock();
                if(DREQ->getLevel()) //can we send 32 bytes?
                {
                    deselectxDCS();
                    selectDCS(); 
                    //send 32 bytes
                    for(int i =0; i<32; i++)
                    {
                        m_SPIDrv->transferMP3(*buf_ptr);
                        buf_ptr++;
                    }
                    deselectDCS();    
                }
                spi1_unlock();
            }
        }
        if(itsPlayBackControl->getCurrentState() == STOPPED)
        {
            //u0_dbg_printf("Decoder STOP\n");
            vTaskSuspend(NULL);
        } 
    }
}

bool VS1011Drv::stopSong()
{
        //   -Read endFillbyte
        // -send 2052 bytes of endFillByte
        // -set SCI_Mode bit CANCEL
        // -Send 32 bytes of endFillByte
        // -If sm_cancel is still not cleared do a soft reset.
        uint16_t endFillByte;
        SendSCIWriteCommand(SCI_WRAMADDR, EDNFB_ADDR);
        endFillByte = SendSCIReadCommand(SCI_WRAM);
        bool cancelCleared = false;
    
        deselectxDCS();
        selectDCS();
        for(int i =0; i<2052; i++)
        {
            //send 2052 bytes of end fill bytes
            m_SPIDrv->transferMP3(endFillByte);
        }
        deselectDCS();

        SendSCIWriteCommand(0x00, 0x0800 | 0x0008); //set cancel bit
        while(!cancelCleared)
        {
            cancelCleared = !(SendSCIReadCommand(0x0) & (1<<3));
            selectDCS();
            for(uint8_t i = 0; i<32; i++)
            {
                m_SPIDrv->transferMP3(endFillByte);
            }
            deselectDCS();
        }
        u0_dbg_printf("SONG STOPPED \n");
        return true;
}

//This is used for initial setup, sending config commands to the flash
void VS1011Drv::SendSCIWriteCommand(uint8_t regAddr, uint16_t writeData)
{
    uint8_t const SCI_WRITE = 0x02;
    uint8_t highByte = (writeData >> 8);
    uint8_t lowByte = writeData;
    deselectDCS();
    selectxDCS();

    m_SPIDrv->transferMP3(SCI_WRITE);    //send write command
    m_SPIDrv->transferMP3(regAddr);
    m_SPIDrv->transferMP3(highByte);
    m_SPIDrv->transferMP3(lowByte);

    while(!DREQ->getLevel());
    deselectxDCS();
}

uint16_t VS1011Drv::SendSCIReadCommand(uint8_t regAddr)
{
    deselectDCS();
    selectxDCS();

    m_SPIDrv->transferMP3(0x03);
    m_SPIDrv->transferMP3(regAddr);
    uint16_t high = m_SPIDrv->transferMP3(0xff);
    uint16_t low = m_SPIDrv->transferMP3(0xff);

    while(!DREQ->getLevel());
    deselectxDCS();

    return (high << 8) | low;
}

void VS1011Drv::setVolume(uint16_t db)
{
    //this function expects a value from 0- 511?
    spi1_lock();
    // uint16_t newdB = (db/0.5) * 256 + (db/0.5);
    printf("new volume:%x \n",db);
    SendSCIWriteCommand(0xB, db);
    spi1_unlock();
}
//sets the member variable queue handler to the one created by the 
//task
void VS1011Drv::setQueueHandler(QueueHandle_t &flashQueue)
{
    m_transmitQueue = &flashQueue;
}

void VS1011Drv::selectDCS()
{
    DCS->setLow();
}

void VS1011Drv::deselectDCS()
{
    DCS->setHigh();
}

void VS1011Drv::selectxDCS()
{
    xDCS->setLow();
}

void VS1011Drv::deselectxDCS()
{
    xDCS->setHigh();
}