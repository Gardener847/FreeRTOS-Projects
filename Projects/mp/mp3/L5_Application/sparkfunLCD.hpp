#ifndef SPARKFUNLCD_HPP__
#define SPARKFUNLCD_HPP__

#include <stdio.h>
#include <stddef.h>
#include "utilities.h"
#include "io.hpp"
#include "MP3GPIO.hpp"
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "task.h"
#include "uart0_min.h"
#include "printf_lib.h"
#include "LPC17xx.h"
#include <string.h>
#include "Flash.h"
#include "labSPI.hpp"
// #include "buttonsAndGlobals.hpp"
enum buttonPressed
{
	PLAY_PAUSE,
	SELECT,
	DOWN,
	BACK,
	SKIP_FORWARD,
	SKIP_BACK
};

class sparkfunLCD 
{
    public:
        sparkfunLCD();
        ~sparkfunLCD();

        void nowPlayingStateHandle(buttonPressed &buttonINT);
        void mainMenuStateHandle(buttonPressed &buttonINT);
        void songsStateHandle(buttonPressed &buttonINT);
        void artistsStateHandle(buttonPressed &buttonINT);
        void HandleButtonINT(buttonPressed &buttonINT);
        void displayCurrentState();

        void getMP3data();

        void spiLCDInit();

        void spiSendString(char *d);

        void newLine();

        void clearDisplay();

        void resetTimer();

        void classInit();

        MP3GPIO lcd_cs;  // lcd_cs(1, 0);                 
        
        bool songIsPlaying;

        typedef enum 
        {
            mainMenuScreen,
            songsScreen,
            artistsScreen,
            nowPlayingScreen
        } myStateType;

        myStateType currentState = mainMenuScreen;
        uint8_t currentPlayingSong;
        VECTOR<SongStruct> * playingSongDirectory;

    private:

        VECTOR<ArtistStruct> library_artists;
        VECTOR<SongStruct> library_songs;
        VECTOR<SongStruct> * currentSongDirectory;
        
        uint8_t menu_selectedOption;
        uint8_t songs_selectedOption;
        uint8_t artists_selectedOption;
        uint8_t nowPlaying_selectedOption;
        str nowPlaying_selectedString;

        int seconds;
        int minute;
        int bytes2write;
        char data[20];

        LabSPI instance;
        

        VECTOR<str> menu;
           
};

#endif