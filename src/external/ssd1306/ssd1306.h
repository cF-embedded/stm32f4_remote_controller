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

/* SSD1306 Height in pixels */
#define SSD1306_HEIGHT 64
/* SSD1306 Width in pixels */
#define SSD1306_WIDTH 128

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
 * @param pBMP
 */
void ssd1306_draw_bitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);

#endif   // __SSD1306_H__