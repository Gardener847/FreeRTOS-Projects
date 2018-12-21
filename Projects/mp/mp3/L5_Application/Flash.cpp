#include <stdio.h>              // u0_dbg_printf(), sprintf()
#include "lpc_sys.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "printf_lib.h"

#include "storage.hpp"          // Get Storage Device instances
#include "ff.h"

#include "vector.hpp"
#include "Flash.h"
#include <string.h>
#include "str.hpp"

Flash::Flash() {
    numFiles = 0;
}

Flash * Flash::m_instance = NULL;

Flash * Flash::instance()
{
    if(!m_instance)
    {
        m_instance = new Flash();
    }
    return m_instance;
}
uint8_t Flash::get_number_of_songs(){
    return numFiles;
}

void Flash::get_mp3_files() {
    DIR Dir;
    FILINFO Finfo;
    FRESULT returnCode = FR_OK;
    
    #if _USE_LFN
        char Lfname[_MAX_LFN];
    #endif
    // access SD directory directly
    const char *dirPath = "1:";
    if (FR_OK != (returnCode = f_opendir(&Dir, dirPath))) {
        u0_dbg_printf("Invalid directory: |%s| (Error %i)\n", dirPath, returnCode);
        return;
    }

    u0_dbg_printf("Directory listing of: %s\n", dirPath);
    // endless loop
    uint8_t iter = 0;
    for (;;) {
        #if _USE_LFN
            Finfo.lfname = Lfname;
            Finfo.lfsize = sizeof(Lfname);
        #endif

        returnCode = f_readdir(&Dir, &Finfo);
        
        if ((FR_OK != returnCode) || !Finfo.fname[0]) {
            break;
        }
        else {
            char tmp[20];
            sprintf(tmp, Finfo.fname);
            str name = tmp;
            // check if currently reading mp3 file
            if (name.contains("MP3")) {
                ++numFiles;
                sprintf(mp3Files[iter], Finfo.lfname);

                ++iter;
            }
        }
    }
    f_closedir(&Dir);
    return;
}

//okay...
//This takes an index, to get the current file
//It reads that files metadata and fills the contents of the artist and song struct.
void Flash::get_mp3_metadata(int k, ArtistStruct& artist, SongStruct& song) {
    char fileName[100];
    char buffTmp[100];
    uint16_t i = 0;

    str file = "1:";
    str tmp1(mp3Files[k]);
    file.append(tmp1);
    song.fileName = file;
    strcpy(fileName, file.c_str());
    u0_dbg_printf("%s\n",song.fileName.c_str());
    // fixes weird bug that ignores metadata of first mp3 file
    for (i = 0; i < 75; i++) {
        buffTmp[i] = ' ';
    }

    Storage::read(fileName, &buffTmp, strlen(buffTmp), 0);

    // clean buffer by replacing weird characters with space
    for (i = 0; i < 100; i++) 
    {
        if (buffTmp[i] > 122 || (buffTmp[i] < 97 && buffTmp[i] > 90) || 
            (buffTmp[i] < 65 && buffTmp[i] > 57) || (buffTmp[i] < 48 && buffTmp[i] > 32) || 
            buffTmp[i] < 32)
            buffTmp[i] = ' ';
    }

    for (i = 0; i < 100; i++) 
    {
        int j=0;
        // navigate to Song Title
        if (buffTmp[i] == '2' && buffTmp[i - 1] == 'T') 
        {
            // skip non alpha
            while (buffTmp[i] > 122 || buffTmp[i] < 65) 
            {
                i++;
            }

            // get song title
            while (1) 
            {
                song.songName += buffTmp[i];
                //u0_dbg_printf("%c  %d \n",song.songName[j], song.songName.getCapacity());
                i++;
                j++;
                
                if ((buffTmp[i] == 'T') && (buffTmp[i + 1] == 'P') && (buffTmp[i + 2] == 'E'))
                {
                    break;
                }
            }

            // skip tag TPE2, currently at "T"
            i += 4;
            // skip non alpha
            while (buffTmp[i] > 122 || buffTmp[i] < 65) 
            {
                i++;
            }

            j=0;
            // get artist
            //u0_dbg_printf("Artist\n");
            while (1) 
            {
                artist.artistName += buffTmp[i];
                //u0_dbg_printf("%c  %d \n",artist.artistName[j], artist.artistName.getCapacity());
                
                i++;
                j++;
                if ((buffTmp[i] == 'P') && ((buffTmp[i + 1] == ' ') || (buffTmp[i+1] == 'O'))) 
                {
                    break;
                }
            }
            break;
        }
    }
    // str songTmp(songTemp);
    // str artistTmp(artistTemp);
    // song.songName.append(songTmp);
    // artist.artistName.append(artistTmp);
    song.artistName = artist.artistName;
    //u0_dbg_printf("artist:%s  song:%s \n",artist.artistName.c_str(), song.songName.c_str());
    return;
}

void Flash::mp3_init() {
    get_mp3_files();
//  get_mp3_metadata(songs, artists, rows, cols);
    u0_dbg_printf("end of flash init \n");
}

void Flash::send_mp3_file_2_decoder() {
    while(1)
    {
        char songName[30];
        if(xQueueReceive(*songQueue, songName, portMAX_DELAY))
        {
            FIL src_file;
            // Open existing file
            // str fileName;
            // fileName.append(songName);
            // strcpy(songName, fileName.c_str());
            //u0_dbg_printf("songName:%s \n",songName);
            FRESULT status = f_open(&src_file, songName , FA_OPEN_EXISTING | FA_READ);
            if (status == FR_OK) 
            {
                int file_size = f_size(&src_file);
                //u0_dbg_printf("size of mp3: %d \n",file_size);

                // read and send entire file to decoder, may be faster than by going byte by byte
                char buffer[512] = {0};     // extra char for null terminator
                unsigned int bytesRead = 0;
                int totalBytesRead = 0;
                // start sending bytes to decoder
                while ((FR_OK == f_read(&src_file, buffer, sizeof(buffer), &bytesRead)) && (itsPlayBackControl->getCurrentState()!= STOPPED)) 
                {
                    
                    if(itsPlayBackControl->getCurrentState() == PAUSED)
                    {
                        //u0_dbg_printf("Flash PAUSE\n");
                        vTaskSuspend(NULL);
                    }
                    //u0_dbg_printf("WAT \n");
                    xQueueSend(*decoderQueue, buffer, 200);
                    totalBytesRead += bytesRead;
                    if(totalBytesRead >= file_size)
                    {
                        //u0_dbg_printf("WAT \n");
                        break;
                    }
                }

                VS1011Drv::instance()->stopSong();
                f_close(&src_file);

                if(itsPlayBackControl->getCurrentState() == STOPPED)    //We want to play another song. Clear queue
                {
                    //u0_dbg_printf("Flash STOP\n");
                    xQueueReset(*decoderQueue);     //clear the buffer of the previously playing song
                    vTaskSuspend(NULL);
                }
            }
            else 
            {
                u0_dbg_printf("Error reading file");
            }
        }
    }
}

void Flash::setQueueHandlers(QueueHandle_t &dQueue, QueueHandle_t &sQueue)
{
    decoderQueue = &dQueue;
    songQueue = &sQueue;
}