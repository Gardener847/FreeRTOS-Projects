/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 */

#include <stdio.h>
#include "utilities.h"
#include "io.hpp"
#include "sparkfunLCD.hpp"
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "task.h"
#include "uart0_min.h"
#include "VS1011Drv.hpp"
#include "periodic_callback.h"
#include "lpc_sys.h"
#include "MP3GPIO.hpp"
#include "ADCDrv.hpp"

TaskHandle_t viewController;

QueueHandle_t buttonQueue = NULL;
SemaphoreHandle_t playBckCtrlSem = NULL;

sparkfunLCD * myLCD;

MP3GPIO buttonForward;
MP3GPIO buttonBack;

TaskHandle_t flashHandle = NULL;
TaskHandle_t decoderHandle = NULL;
PlayBackControl itsPlayBackControl;

ADCDrv adc_driver;

int volPot = 0;
void volUpDown(void *pvParams)
{
	char * result = new char[32];
	int lastReadPot = (uint16_t)adc_driver.ReadAdcVoltageByChannel(3);
	const uint8_t voltageSteps = 51;
	const uint8_t volumeStep = 0x05;
	while (1)
	{
		int  currentReadPot = (int)adc_driver.ReadAdcVoltageByChannel(3);
		uint16_t result;
		// threshold de-bouncer: potentiometer has range 0 to 4096, mp3 volume has 0 to 254
		if ((currentReadPot > lastReadPot + 80) || currentReadPot < (lastReadPot-80))
		{
			result = (volumeStep*(currentReadPot/80) << 8) | (volumeStep*(currentReadPot/80)); 
			lastReadPot = currentReadPot;
			VS1011Drv::instance()->setVolume(result);
		}

		vTaskDelay(500);
	}
}
//producer
void vQueueMp3Files(void * ) 
{
	u0_dbg_printf("starting flash \n");
	Flash::instance()->send_mp3_file_2_decoder();
}
//consumer
void vSendToDecoder(void *)
{
	u0_dbg_printf("starting Decoder \n");
	VS1011Drv::instance()->SendToDecoder();
}

//This task controls the playback of the MP3
void vControlMP3playback(void *)
{
	str currentlyPlayingFile;
	while(1)
	{	
		u0_dbg_printf("controlPlay \n");
		if(xSemaphoreTake(playBckCtrlSem, portMAX_DELAY))
		{
			//we can safely assume that the UI has updated along with all necessary state variables at the time of this tasks execution
			//if the last time we saved the playing file, the song has changed, we know we need to stop to prepare for SKIP_FORWARD, SKIP_BACK, 
			u0_dbg_printf("%s, %s,\n",currentlyPlayingFile.c_str(), myLCD->playingSongDirectory->at(myLCD->currentPlayingSong).fileName.c_str());
			if(currentlyPlayingFile != myLCD->playingSongDirectory->at(myLCD->currentPlayingSong).fileName)
			{
				currentlyPlayingFile = myLCD->playingSongDirectory->at(myLCD->currentPlayingSong).fileName;
				u0_dbg_printf("change song\n");
				if(itsPlayBackControl.currentState == PAUSED)
				{
					itsPlayBackControl.currentState = STOPPED;
					vTaskResume(decoderHandle);
					vTaskResume(flashHandle);	
				}
				itsPlayBackControl.currentState = STOPPED;
				vTaskDelay(500);
				itsPlayBackControl.currentState = PLAYING;

				vTaskResume(decoderHandle);
				vTaskResume(flashHandle);
				xQueueSend(*Flash::instance()->songQueue, currentlyPlayingFile.c_str(), portMAX_DELAY);
			}
			else 	//song is same song, so we must be wanting to pause
			{
				if(itsPlayBackControl.currentState == PAUSED)
				{
					u0_dbg_printf("PAUSED to PLAY\n");
					itsPlayBackControl.currentState = PLAYING;
					vTaskResume(decoderHandle);
					vTaskResume(flashHandle);
				}
				else if(itsPlayBackControl.currentState == PLAYING)
				{
					u0_dbg_printf("PLAY to PAUSE \n");
					itsPlayBackControl.currentState = PAUSED;
				}
			}
			u0_dbg_printf("playing %s \n", currentlyPlayingFile.c_str());
		}
	}
}
void vControlLED(void *)
{
	while(1)
    	{
		buttonPressed buttonINT;
		if(xQueueReceive(buttonQueue, &buttonINT, portMAX_DELAY))
		{

			myLCD->HandleButtonINT(buttonINT);	//update the state machine
			myLCD->displayCurrentState();	//update the LCD according to the state

			//Synchronize with the plabackCTL if necessary
			if(buttonINT == PLAY_PAUSE || buttonINT == SKIP_FORWARD || buttonINT == SKIP_BACK 
			|| (buttonINT == SELECT && myLCD->currentState == sparkfunLCD::nowPlayingScreen))
			{
				xSemaphoreGive(playBckCtrlSem);
			}
		}
    }
}

void vReadSwitches(void *)
{
	int play_pause = PLAY_PAUSE;
	int select = SELECT;
	int down = DOWN;
	int back = BACK;
	int skip_back = SKIP_BACK;
	int skip_forward = SKIP_FORWARD;

	//wait 50 ticks to have our button press registered.
	while(1)
	{
		//u0_dbg_printf("switches task \n");
		if(SW.getSwitch(1))
		{
			//u0_dbg_printf("switch1 \n");
			xQueueSend(buttonQueue, &play_pause, 150);
		}
		else if(SW.getSwitch(2))
		{
			//u0_dbg_printf("switch2 \n");
			xQueueSend(buttonQueue, &select, 150);
		}
		else if(SW.getSwitch(3))
		{
			//u0_dbg_printf("switch3 \n");
			xQueueSend(buttonQueue, &down, 150);
		}
		else if(SW.getSwitch(4))
		{
			//u0_dbg_printf("switch4 \n");
			xQueueSend(buttonQueue, &back, 150);
		}
		else if(buttonBack.getLevel())
		{
			xQueueSend(buttonQueue, &skip_back, 150);
		}
		else if(buttonForward.getLevel())
		{
			xQueueSend(buttonQueue, &skip_forward, 150);
		}
		vTaskDelay(150);
	}
}

int main (void)
{
	buttonQueue = xQueueCreate(1, sizeof(int));
	playBckCtrlSem = xSemaphoreCreateBinary();
	QueueHandle_t decoderQueue = xQueueCreate(2, 512);	//buffer for sending mp3 to decoder
	QueueHandle_t songQueue = xQueueCreate(1, 30);		//holds the song title we requested
	
	buttonForward.initialize(6,2);
	buttonBack.initialize(7,2);
	// buttonForward.setLow();
	// buttonBack.setLow();
	buttonForward.setAsInput();
	buttonBack.setAsInput();

	myLCD = new sparkfunLCD();

	Flash::instance()->mp3_init();
	Flash::instance()->setQueueHandlers(decoderQueue, songQueue);

	myLCD->classInit();

	VS1011Drv::instance()->initialize();
	VS1011Drv::instance()->setQueueHandler(decoderQueue);
	VS1011Drv::instance()->itsPlayBackControl = &itsPlayBackControl;
	Flash::instance()->itsPlayBackControl = &itsPlayBackControl;

	adc_driver.AdcInitBurstMode();
	adc_driver.AdcSelectPin(ADCDrv::k0_26);
	char memInfo[280];
	sys_get_mem_info_str(memInfo);
	printf("%s",memInfo);
	BaseType_t xReturned;
	xReturned = xTaskCreate(vControlLED, "vControlLED", 512, NULL, tskIDLE_PRIORITY+4,  &viewController);
	//  if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	// 	u0_dbg_printf("Failed to create mem 1 \n");

    xReturned = xTaskCreate(vReadSwitches, "vReadSwitches", 256, NULL, tskIDLE_PRIORITY+3, NULL);
	//  if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	// 	u0_dbg_printf("Failed to create mem  2\n");

	xReturned =  xTaskCreate(vSendToDecoder, "vSendToDecoder", 512, NULL, tskIDLE_PRIORITY+2, &flashHandle);
	// if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	// 	u0_dbg_printf("Failed to create mem  3\n");

	xReturned =  xTaskCreate(vQueueMp3Files, "vqMp3", 512, NULL, tskIDLE_PRIORITY+1, &decoderHandle);
	// if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	// 	u0_dbg_printf("Failed to create mem  4\n");
		
	 xReturned =  xTaskCreate(vControlMP3playback,"vControlMP3playback", 512, NULL, tskIDLE_PRIORITY+3, NULL);
	// if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);
	// 	u0_dbg_printf("Failed to create mem  5\n");

	xTaskCreate(volUpDown, "vVolUpDown", 256, NULL, tskIDLE_PRIORITY+3, NULL);
	sys_get_mem_info_str(memInfo);
	printf("%s",memInfo);
    // xTaskCreate(timerTask, "timerTask", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    //scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    vTaskStartScheduler();
	u0_dbg_printf("woops");
    return 0;
}
