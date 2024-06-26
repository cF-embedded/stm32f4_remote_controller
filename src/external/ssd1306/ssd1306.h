/**
 * @file ssd1306.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief Driver for ssd1306 with I2C and FreeRTOS
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "platform_specific.h"

#define SSD1306_BLACK 0   //< Draw 'off' pixels
#define SSD1306_WHITE 1   //< Draw 'on' pixels

/* Screen orientation */
#define SSD1306_HORIZONTAL_MODE1 0
#define SSD1306_VERTICAL         1
#define SSD1306_HORIZONTAL_MODE2 2

#define SSD1306_MEMORYMODE          0x20   //< See datasheet
#define SSD1306_COLUMNADDR          0x21   //< See datasheet
#define SSD1306_PAGEADDR            0x22   //< See datasheet
#define SSD1306_SETCONTRAST         0x81   //< See datasheet
#define SSD1306_CHARGEPUMP          0x8D   //< See datasheet
#define SSD1306_SEGREMAP            0xA0   //< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4   //< See datasheet
#define SSD1306_DISPLAYALLON        0xA5   //< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6   //< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7   //< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8   //< See datasheet
#define SSD1306_DISPLAYOFF          0xAE   //< See datasheet
#define SSD1306_DISPLAYON           0xAF   //< See datasheet
#define SSD1306_COMSCANINC          0xC0   //< Not currently used
#define SSD1306_COMSCANDEC          0xC8   //< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3   //< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5   //< See datasheet
#define SSD1306_SETPRECHARGE        0xD9   //< See datasheet
#define SSD1306_SETCOMPINS          0xDA   //< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB   //< See datasheet
#define SSD1306_DEACTIVATE_SCROLL   0x2E

#define SSD1306_SETSTARTLINE 0x40   //< See datasheet

/**
 * @brief Initalization ssd1306
 *
 */
void ssd1306_init(void);

/**
 * @brief Set all pixels as black color
 *
 * @param color
 */
void ssd1306_clear_screen(void);

/**
 * @brief Send data from buffer to screen memory
 *
 */
void ssd1306_update_screen(void);

/**
 * @brief Draw one white pixel in the selected position
 *
 * @param x
 * @param y
 * @param color
 */
void ssd1306_draw_pixel(uint8_t x, uint8_t y);

/**
 * @brief Draw bitmap in the selected position
 *
 * @param X
 * @param Y
 * @param W
 * @param H
 * @param bitmap
 */
void ssd1306_draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap);

/**
 * @brief Drawing string in the selected position
 *
 * @param x
 * @param y
 * @param c
 */
void ssd1306_draw_string(uint8_t x, uint8_t y, char* c);

#endif   // __SSD1306_H__