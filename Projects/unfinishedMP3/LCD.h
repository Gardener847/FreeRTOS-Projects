#pragma once
#ifndef LCD_H
#define LCD_H

class LCD{
    public:
        LCD();
        ~LCD();
        void lcd_init(uint8_t port_num, uint8_t data_size_select, FrameModes format, uint8_t divide);
        void draw_or_type(uint8_t c);
        void spiwrite(uint8_t c);
};

#endif