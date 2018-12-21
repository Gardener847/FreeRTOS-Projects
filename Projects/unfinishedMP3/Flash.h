#pragma once
#ifndef FLASH_H
#define FLASH_H

#include "queue.h"

#include "vector.hpp"
#include "str.hpp"

struct FileName {
    char buffer[20]
};

class Flash {
    private:
        // for dislay output
        VECTOR<string> mp3Meta;
        // for opening and reading files
        VECTOR<FileName> mp3Files;
        uint8_t numFiles;
    public:
        Flash();
        // to be used with LCD
        void get_mp3_meta_data();
        void print_meta_data();
        // if user is ready to switch, or start, the song,
        // have user specify the song to send to decoder with
        // file number (index inside the vector)
        bool send_mp3_file_2_decoder(uint8_t songIndex, QueueHandle_t *queue);
};
#endif