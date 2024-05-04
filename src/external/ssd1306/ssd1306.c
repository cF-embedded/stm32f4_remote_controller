/**
 * @file ssd1306.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief Driver for ssd1306 with I2C and FreeRTOS
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ssd1306.h"
#include "i2c_master.h"
#include <string.h>

/* I2C adress of ssd1306 display */
#define SSD1306_I2C_ADRESS 0x3C

/* SSD1306 Width in pixels */
#define SSD1306_WIDTH 128
/* SSD1306 Height in pixels */
#define SSD1306_HEIGHT 64

/* Special char to send before command */
#define SSD1306_COMMAND 0x00

/* Special char to send before data */
#define SSD1306_DATA 0x40

/* Buffer to contain pixels color on oled display*/
#define BUFFER_SIZE (SSD1306_WIDTH * (SSD1306_HEIGHT / 8) + 1)

/* buffer to hold sent pixels */
/* first element = SSD1306_DATA */
static uint8_t buffer[BUFFER_SIZE];

/**
 * @brief Write command to ssd1306 by I2C
 *
 * @param command
 */
static void ssd1306_write_command(uint8_t command);

/**
 * @brief  Write data to ssd1306 by I2C
 *
 * @param ptr_data
 * @param data_len
 */
static void ssd1306_write_data(uint8_t* data, size_t data_len);

void ssd1306_init(void)
{
    ssd1306_write_command(SSD1306_DISPLAYOFF);

    ssd1306_write_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_write_command(0x80);

    ssd1306_write_command(SSD1306_SETMULTIPLEX);
    ssd1306_write_command(SSD1306_HEIGHT - 1);

    ssd1306_write_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_write_command(0x00);

    ssd1306_write_command(SSD1306_SETSTARTLINE);

    ssd1306_write_command(SSD1306_CHARGEPUMP);
    ssd1306_write_command(0x14);

    ssd1306_write_command(SSD1306_MEMORYMODE);
    ssd1306_write_command(0x00);

    ssd1306_write_command(SSD1306_SEGREMAP | 0x10);
    ssd1306_write_command(SSD1306_WIDTH - 1);

    ssd1306_write_command(SSD1306_COMSCANDEC);

    ssd1306_write_command(SSD1306_SETCOMPINS);
    ssd1306_write_command(0x12);

    ssd1306_write_command(SSD1306_SETCONTRAST);
    ssd1306_write_command(0x7F);

    ssd1306_write_command(SSD1306_SETPRECHARGE);
    ssd1306_write_command(0xF1);

    ssd1306_write_command(SSD1306_SETVCOMDETECT);
    ssd1306_write_command(0x40);

    ssd1306_write_command(SSD1306_DISPLAYALLON_RESUME);
    ssd1306_write_command(SSD1306_NORMALDISPLAY);
    ssd1306_write_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_clear_screen();

    ssd1306_write_command(SSD1306_DISPLAYALLON);
    ssd1306_write_command(SSD1306_DISPLAYON);
}

void ssd1306_clear_screen(void)
{
    memset(buffer, 0x00, BUFFER_SIZE);

    ssd1306_update_screen();
}

void ssd1306_update_screen(void)
{
    ssd1306_write_command(SSD1306_PAGEADDR);
    ssd1306_write_command(0x00);
    ssd1306_write_command(0xFF);

    ssd1306_write_command(SSD1306_COLUMNADDR);
    ssd1306_write_command(0x00);
    ssd1306_write_command(SSD1306_WIDTH - 1);

    ssd1306_write_data(buffer, BUFFER_SIZE);
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y)
{
    if(x >= (SSD1306_WIDTH + 1) || y >= (SSD1306_HEIGHT + 1))
    {
        /* Prevent writing outside buffer */
        return;
    }

    uint8_t y_offset = (SSD1306_HEIGHT - 1) - y;
    buffer[(x + (y_offset / 8) * SSD1306_WIDTH) + 1] |= (1 << (y_offset & 7));
}

void ssd1306_draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap)
{
    for(int16_t i = 0; i < h; ++i)
    {
        for(int16_t j = 0; j < w; ++j)
        {
            int16_t byteIndex = i * (w / 8) + j / 8;
            int16_t bitIndex = j % 8;
            if(bitmap[byteIndex] & (1 << (7 - bitIndex)))
            {
                ssd1306_draw_pixel(x + j, y + i);
            }
        }
    }
}

static void ssd1306_write_command(uint8_t command)
{
    uint8_t tmp_buf[2];

    tmp_buf[0] = SSD1306_COMMAND;
    tmp_buf[1] = command;

    i2c_master_write(tmp_buf, SSD1306_I2C_ADRESS, 2);
}

static void ssd1306_write_data(uint8_t* data, size_t data_len)
{
    data[0] = SSD1306_DATA;

    i2c_master_write(data, SSD1306_I2C_ADRESS, data_len);
}