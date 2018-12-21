#pragma once
#ifndef LCD_H
#define LCD_H

#include "gfxfont.h"

class LCD {
    private:
        uint8_t *buffer;
    public:
        LCD(int16_t w, int16_t h);
        void transfer(uint8_t send)
        void setCursor(int16_t x, int16_t y);
        void setTextColor(uint16_t c);
        void setTextWrap(boolean w);
        void startWrite(void);
        void endWrite(void);
        void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
        void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void writePixel(int16_t x, int16_t y, uint16_t color);
        void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                        uint16_t bg, uint8_t size);
        void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color);
        void write(uint8_t);
        void fillScreen(uint16_t color);
        void drawText(char *text, uint16_t color);
    protected:
        const int16_t
        WIDTH,          ///< This is the 'raw' display width - never changes
        HEIGHT;         ///< This is the 'raw' display height - never changes
    int16_t
        _width,         ///< Display width as modified by current rotation
        _height,        ///< Display height as modified by current rotation
        cursor_x,       ///< x location to start print()ing text
        cursor_y;       ///< y location to start print()ing text
    uint16_t
        textcolor,      ///< 16-bit background color for print()
        textbgcolor;    ///< 16-bit text color for print()
    uint8_t
        textsize,       ///< Desired magnification of text to print()
        rotation;       ///< Display rotation (0 thru 3)
    boolean
        wrap,           ///< If set, 'wrap' text at right edge of display
        _cp437         ///< If set, use correct CP437 charset (default is off)
    GFXfont
        *gfxFont;       ///< Pointer to special font
};
#endif