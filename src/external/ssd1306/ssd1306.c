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

/* Buffer to contain pixels color on oled display*/
#define BUFFER_SIZE (SSD1306_WIDTH * ((SSD1306_HEIGHT + 7) / 8))

/* buffer to hold sent pixels */
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
    ssd1306_write_command(0xAE);   // Set display OFF

    ssd1306_write_command(0xD5);   // Set Display Clock Divide Ratio / OSC Frequency
    ssd1306_write_command(0x80);   // Display Clock Divide Ratio / OSC Frequency

    ssd1306_write_command(0xA8);   // Set Multiplex Ratio
    ssd1306_write_command(0x3F);   // Multiplex Ratio for 128x64 (64-1)

    ssd1306_write_command(0xD3);   // Set Display Offset
    ssd1306_write_command(0x00);   // Display Offset

    ssd1306_write_command(0x40);   // Set Display Start Line

    ssd1306_write_command(0x22);   // Set page address
    ssd1306_write_command(0x00);   // Start page address
    ssd1306_write_command(0x07);   // End page address

    ssd1306_write_command(0x21);                // Set column address
    ssd1306_write_command(0x00);                // Start column address
    ssd1306_write_command(SSD1306_WIDTH - 1);   // End column address

    ssd1306_write_command(0x8D);   // Set Charge Pump
    ssd1306_write_command(0x14);   // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    ssd1306_write_command(0xA1);   // Set Segment Re-Map
    ssd1306_write_command(0xC8);   // Set Com Output Scan Direction

    ssd1306_write_command(0xDA);   // Set COM Hardware Configuration
    ssd1306_write_command(0x12);   // COM Hardware Configuration

    ssd1306_write_command(0x81);   // Set Contrast
    ssd1306_write_command(0xFF);   // Contrast

    ssd1306_write_command(0xD9);   // Set Pre-Charge Period
    ssd1306_write_command(0xF1);   // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    ssd1306_write_command(0xDB);   // Set VCOMH Deselect Level
    ssd1306_write_command(0x40);   // VCOMH Deselect Level

    ssd1306_write_command(0xA4);   // Set all pixels OFF
    ssd1306_write_command(0xA6);   // Set display not inverted
    ssd1306_write_command(0xAF);   // Set display On

    /* Clear screen */
    ssd1306_clear_screen();
}

void ssd1306_clear_screen(void)
{
    memset(buffer, 0xFF, BUFFER_SIZE);
}

void ssd1306_update_screen(void)
{
    ssd1306_write_command(0x22);
    ssd1306_write_command(0x00);   // Start page address
    ssd1306_write_command(0x07);   // End page address

    ssd1306_write_command(0x21);
    ssd1306_write_command(0x00);                // Start column address
    ssd1306_write_command(SSD1306_WIDTH - 1);   // End column address

    ssd1306_write_data(buffer, BUFFER_SIZE);
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y)
{
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        /* Prevent writing outside buffer */
        return;
    }

    buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y & 7));
}

void ssd1306_DrawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)
{
    uint8_t pX;
    uint8_t pY;
    uint8_t tmpCh;
    uint8_t bL;

    pY = Y;
    while(pY < Y + H)
    {
        pX = X;
        while(pX < X + W)
        {
            bL = 0;
            tmpCh = *pBMP++;
            if(tmpCh)
            {
                while(bL < 8)
                {
                    if(tmpCh & 0x01)
                        ssd1306_draw_pixel(pX, pY + bL);
                    tmpCh >>= 1;
                    if(tmpCh)
                        bL++;
                    else
                    {
                        pX++;
                        break;
                    }
                }
            }
            else
                pX++;
        }
        pY += 8;
    }
}

static void ssd1306_write_command(uint8_t command)
{
    i2c_master_write(&command, SSD1306_I2C_ADRESS, 1);
}

static void ssd1306_write_data(uint8_t* data, size_t data_len)
{
    i2c_master_write(data, SSD1306_I2C_ADRESS, data_len);
}