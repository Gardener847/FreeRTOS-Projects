#include "sparkfunLCD.hpp"
#include "MP3GPIO.hpp"

sparkfunLCD::sparkfunLCD()
{
    uart0_puts("begin constructor\n");
    lcd_cs.initialize(1, 0);

	menu.push_back("Main Menu");
	menu.push_back(">Songs");
	menu.push_back(">Artists");
	menu.push_back(">Now Playing");

	currentPlayingSong = 0;
	menu_selectedOption = 1;
	songs_selectedOption = 0;
	artists_selectedOption = 0;
	nowPlaying_selectedOption = 0;
    songIsPlaying = 0;

    seconds = 0;
    minute = 0;
    bytes2write = 0;

    uart0_puts("end LCDconstructor\n");
}

void sparkfunLCD::classInit()
{
    uart0_puts("class init\n");
	getMP3data();

	lcd_cs.setAsOutput();

	spiLCDInit();
	displayCurrentState();
}

void sparkfunLCD::spiLCDInit()
{
    // outputSetup();
    // instance.initialize(8, MP3SPI::SPI, 128);
	instance.initialize();
    delay_ms(200);

    lcd_cs.setLow();
    instance.transfer(0x7c);	// setting mode
    instance.transfer(0x2d);	// clearing
    lcd_cs.setHigh();
    
    lcd_cs.setLow();
    instance.transfer(0x7c);
    instance.transfer(0x10);	// baud rate
    lcd_cs.setHigh();

    delay_ms(100);
}

void sparkfunLCD::spiSendString(char *d)
{
    lcd_cs.setLow();
	int i = 0;
	while (d[i] != '\0')
	{
		delay_ms(5);
		instance.transfer(d[i]);
		i++;
	}
	lcd_cs.setHigh();
}

void sparkfunLCD::newLine()
{
    lcd_cs.setLow();
	instance.transfer(0x0d);
	lcd_cs.setHigh();
}

void sparkfunLCD::clearDisplay()
{
    lcd_cs.setLow();
	instance.transfer(0x7c);
	instance.transfer(0x2d);
	lcd_cs.setHigh();
}

void sparkfunLCD::resetTimer()
{
    seconds = 0;
	minute = 0;
}

/*state variables needed for menu
    -MENU listing
    -selectedOption
    -

TODO:
    resume and pause timer on PAUSE
    -PLAY_PAUSE and SKIP seem to be universal except in the now playing state, where we actually
    need to make changes.
*/

void sparkfunLCD::mainMenuStateHandle(buttonPressed &buttonINT)
{
	//u0_dbg_printf("main: %d\n",buttonINT);
	switch(buttonINT)
	{
		case PLAY_PAUSE:
			//should do nothing
            //set song to play or pause
            songIsPlaying = (songIsPlaying)? 0:1;
			break;
		case SELECT:
			//go to the state menu context indicated by "current" selection
            currentState = static_cast<myStateType>(menu_selectedOption);
            if(menu_selectedOption == songsScreen)
            {
                //we need to discern whether or not we are in the aggregate song library
                //or within a song listing for an artist.
                //if this is selected then we are within the aggregate song library
                songs_selectedOption = 0;
                currentSongDirectory = &library_songs;
            }
            break;
        case DOWN:
            menu_selectedOption++;
            if(menu_selectedOption > menu.size()-1)
            {
                menu_selectedOption = 1;
            }
            break;
		case BACK:
			//do nothing
            break;
		case SKIP_BACK:
            //we need to update the now playing
            currentPlayingSong--;
            if(currentPlayingSong < 0)
            {
                currentPlayingSong = 0;
            }        
            //reset timer
            break;
		case SKIP_FORWARD:
            //update now playing
            currentPlayingSong++;
            if(currentPlayingSong > playingSongDirectory->size()-1)
            {
                currentPlayingSong = 0;
            }    
            break;
	}
}
void sparkfunLCD::songsStateHandle(buttonPressed &buttonINT)
{

	//u0_dbg_printf("songs: %d\n",buttonINT);

	switch(buttonINT)
	{
		case PLAY_PAUSE:
            //set song play or pause
            songIsPlaying = (songIsPlaying)? 0:1;
            break;
		case SELECT:
            currentPlayingSong = songs_selectedOption;
            playingSongDirectory = currentSongDirectory;	//we update the directory in which we are playing songs from
			currentState = nowPlayingScreen;
            break;
        case DOWN:
            songs_selectedOption++;
            if(songs_selectedOption > currentSongDirectory->size()-1)
            {
                songs_selectedOption = 0;
            }
            break;
		case BACK:
            currentState = mainMenuScreen;
            songs_selectedOption = 0;
            break;
		case SKIP_BACK:
            //we need to update the now playing
            currentPlayingSong--;
            if(currentPlayingSong < 0)
            {
                currentPlayingSong = 0;
            }
            break;
		case SKIP_FORWARD:
            //we need to update the now playing
            currentPlayingSong++;
            if(currentPlayingSong > playingSongDirectory->size()-1)
            {
                currentPlayingSong = 0;
            }
            break;
	}
}
void sparkfunLCD::artistsStateHandle(buttonPressed &buttonINT)
{
	//u0_dbg_printf("artists: %d \n",buttonINT);

	switch(buttonINT)
	{
		case PLAY_PAUSE:
            songIsPlaying = (songIsPlaying)? 0:1;
            break;
		case SELECT:
            //we just selected an artist
            songs_selectedOption = 0;   //place selector at first song
            currentSongDirectory = library_artists[artists_selectedOption].songs;  //keep track of song directory we are in
			//u0_dbg_printf("artist's first song: %s \n", library_artists[artists_selectedOption].songs->at(0).songName.c_str());
            currentState = songsScreen;
            break;
        case DOWN:
            artists_selectedOption++;
            if(artists_selectedOption > library_artists.size()-1)
            {
                artists_selectedOption = 0;
            }
            break;
		case BACK:
            currentState = mainMenuScreen;
            artists_selectedOption = 0;
            break;
		case SKIP_BACK:
            currentPlayingSong--;
            if(currentPlayingSong < 0)
            {
                currentPlayingSong = 0;
            }
            break;
		case SKIP_FORWARD:
            currentPlayingSong++;
            if(currentPlayingSong > playingSongDirectory->size()-1)
            {
                currentPlayingSong = 0;
            }
            break;
	}
}
void sparkfunLCD::nowPlayingStateHandle(buttonPressed &buttonINT)
{
	//u0_dbg_printf("nowPlaying: %d \n", buttonINT);
	switch(buttonINT)
	{
		case PLAY_PAUSE:
            songIsPlaying = (songIsPlaying)? 0:1;
            break;
		case SELECT:
            //do nothing
            break;
        case DOWN:
            //show song, then show artist (loop)
            nowPlaying_selectedOption++;
            if(nowPlaying_selectedOption == 2)
            {
                nowPlaying_selectedOption = 0;
                //we want to show the song name
            }
            break;
		case BACK:
            nowPlaying_selectedOption = 0;
            currentState = mainMenuScreen;
            break;
		case SKIP_BACK:
            currentPlayingSong--;
            if(currentPlayingSong < 0)
            {
                currentPlayingSong = 0;
            }
            break;
		case SKIP_FORWARD:
            currentPlayingSong++;
            if(currentPlayingSong > playingSongDirectory->size()-1)
            {
                currentPlayingSong = 0;
            }
            break;
	}
}
//we need to handle the button press according to our current menu context
void sparkfunLCD::HandleButtonINT(buttonPressed &buttonINT)
{   
    //PLAY_PAUSE and SKIP seem to have the same effect in every state
    switch (currentState)
	{
		case mainMenuScreen:
		{
            mainMenuStateHandle(buttonINT);
            break;
		}

		case songsScreen:
		{
            songsStateHandle(buttonINT);
            break;
		}

		case artistsScreen:
		{
            artistsStateHandle(buttonINT);
            break;
		}

		case nowPlayingScreen:
		{
            nowPlayingStateHandle(buttonINT);
			break;
		}
	}
}


//this updates the LCD according to the state variables
void sparkfunLCD::displayCurrentState()
{
    switch(currentState)
    {
        case mainMenuScreen:
		{
            clearDisplay();
            spiSendString((char*)menu.at(0).c_str());
            newLine();
            spiSendString((char*)menu.at(menu_selectedOption).c_str());
            break;
		}

		case songsScreen:
		{
            clearDisplay();
            spiSendString("Songs: ");
            newLine();
            //Send the currently "hovered over" (selected) song
			//u0_dbg_printf("SELECTED SONG: %s",currentSongDirectory->at(songs_selectedOption).songName.c_str());
            spiSendString((char*)currentSongDirectory->at(songs_selectedOption).songName.c_str());
            break;
		}

		case artistsScreen:
		{
            clearDisplay();
            spiSendString("Artists: ");
            newLine();
            spiSendString((char *)library_artists[artists_selectedOption].artistName.c_str());
            break;
		}

		case nowPlayingScreen:
		{
            clearDisplay();
            spiSendString("Now Playing: ");
            newLine();
			if(playingSongDirectory == NULL)
			{
				break;
			}
            if(nowPlaying_selectedOption == 0)
            {
                nowPlaying_selectedString = playingSongDirectory->at(currentPlayingSong).songName;
            }
            else
            {
                nowPlaying_selectedString = playingSongDirectory->at(currentPlayingSong).artistName;
            }
            spiSendString((char*)nowPlaying_selectedString.c_str());
			break;
		}
    }
}

void sparkfunLCD::getMP3data()
{
	//this function needs to figure out all the songs and artists
	//associated with them
	uint8_t numSongs = Flash::instance()->get_number_of_songs();
	u0_dbg_printf("number of songs:%d \n",numSongs);
	//lets say that get_mp3_metadata takes a fileName, returns the song name and artist name
	//Iterate through all files. Add the song to the total list. Look for an existing artist, if not found, create a new artist, add this song to its list
	for(int i=0; i<numSongs; i++)
	{
		ArtistStruct fileArtist;
		SongStruct fileSong;
		//returns the artist of the ith song, and the song
		Flash::instance()->get_mp3_metadata(i, fileArtist, fileSong);

		library_songs.push_back(fileSong);

		//we need to check library artists to see if we already stored this artist
		bool found =false;
		for(int j=0; j<library_artists.size(); j++)
		{
			if(library_artists[j].artistName == fileArtist.artistName)
			{
				//we have match, add it to the existing artist's songs
				library_artists[i].songs->push_back(fileSong);
				found = true;
				break;
			} 
		}
		if(!found)
		{
			//we didn't find an existing artist for this song so make a new one.
			fileArtist.songs = new VECTOR<SongStruct>();
			fileArtist.songs->push_back(fileSong);
			//u0_dbg_printf("added song: %s \n", fileArtist.songs->at(0).songName.c_str());
			library_artists.push_back(fileArtist);
		}
	}
}

sparkfunLCD::~sparkfunLCD()
{

}