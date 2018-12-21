#pragma once
#ifndef FLASH_H
#define FLASH_H

#include "queue.h"

#include "vector.hpp"
#include "str.hpp"

#include "VS1011Drv.hpp"
struct FileName {
    char buffer[20];
};

struct SongStruct
{
    str songName;
    str artistName;
    str fileName;
};
struct ArtistStruct 
{
    str artistName;
    VECTOR<SongStruct> * songs;			
};

class Flash {
    private:
        Flash();
        // for dislay output

        static Flash * m_instance;
    public:
        static Flash * instance();
        // to be used with LCD
        void mp3_init();
        void get_mp3_files();
        void get_mp3_metadata(int k, ArtistStruct &artist, SongStruct &song);
        uint8_t get_number_of_songs();
        // if user is ready to switch, or start, the song,
        // have user specify the song to send to decoder with
        // file number (index inside the vector)
        void send_mp3_file_2_decoder();

        void setQueueHandlers(QueueHandle_t &decoderQueue, QueueHandle_t &songQueue);
        QueueHandle_t * songQueue;
        QueueHandle_t * decoderQueue;

        char mp3Files[10][20];
        // char songTitles[10][20];
        // char artists[10][20];
        // for opening and reading files
        VECTOR<FileName> mp3Files1;
        uint8_t numFiles;
        PlayBackControl * itsPlayBackControl;
};
#endif
