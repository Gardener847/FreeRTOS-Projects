#include <stdio.h>              // printf(), sprintf()
#include "lpc_sys.h"
#include "FreeRtos.h"
#include "queue.h"
#include "printf_lib.h"
#include "str.hpp"

#include "storage.hpp"          // Get Storage Device instances
#include "ff.h"

#include "vector.hpp"

Flash::Flash() {
    numFiles = 0;
}

void Flash::get_mp3_meta_data() {
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
        return true;
    }

    u0_dbg_printf("Directory listing of: %s\n\n", dirPath);
    // endless loop
    for (;;) {
        #if _USE_LFN
            Finfo.lfname = Lfname;
            Finfo.lfsize = sizeof(Lfname);
        #endif

        returnCode = f_readdir(&Dir, &Finfo);
        
        if ( (FR_OK != returnCode) || !Finfo.fname[0]) {
            break;
        }
        else {
            string file_name = Finfo.fname[0];
            // check if currently reading mp3 file
            if (Finfo.fname[0].find('3')) {
                numFiles++;
                mp3Meta.push_back(file_name);

                // store name for file referencing when opening, reading, and decoding files
                FileName fileRef;
                sprintf(fileRef.buffer, "1:%s", file_name.c_str());
                mp3Files.push_back(fileRef);
            }
        }
    }
    f_closedir(&Dir);
    return true;
}

bool Flash::send_mp3_file_2_decoder(uint8_t songIndex, QueueHandle_t *queue) {
    if (songIndex < 0 || songIndex > (numFiles - 1)){
        return false;
    }
    else {
        FIL src_file;
        // Open existing file
        FRESULT status = f_open(&src_file, mp3Files[songIndex - 1].buffer, FA_OPEN_EXISTING | FA_READ);
        if (status == FR_OK) {
            // returns number of DWORDs, so 1 DWORD = 4 bytes
            int file_size = f_size(&src_file);

            // read and send file byte by byte to decoder, may be slower than by taking the whole file
            // f_forward: forward data to the stream
            // f_lseek: move file pointer of  afile

            // read and send entire file to decoder, may be faster than by going byte by byte
            char buffer[512] = {0};     // extra char for null terminator
            unsigned int bytesRead = 0;
            // start sending bytes to decoder
            while (FR_OK == f_read(&src_file, buffer, sizeof(buffer), &bytesRead) > 0) {
                // BE SURE TO CREATE A QUEUE INSIDE THE MAIN
                xQueueSend(queue, &src_file, portMAX_DELAY);
            }
            f_close(&src_file);
            return true;
        }
        else {
            u0_dbg_printf("Error reading file");
        }
    }
}

void Flash::print_meta_data() {
    for (auto i : numFiles){
        u0_dbg_printf("File name: \n", mp3Files[i].fname);
    }
}