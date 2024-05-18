/**
 * @file display_state_machine_func.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief Screen functions for display state machine
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "adc.h"
#include "display_battery_bitmap.h"
#include "display_speedometer_bitmap.h"
#include "display_state_machine_types.h"
#include "hm_10.h"
#include "ssd1306.h"
#include <stdlib.h>
#include <string.h>

/** Len of buffer for measured data. */
#define SCREEN_BUF_SIZE 5

/* Array storing displayed measured data */
static char screen_buf[SCREEN_BUF_SIZE];

void display_speedometer_screen(display_screen_s_t* speedometer_screen)
{
    if(hm_10_read_buf((uint8_t*)&speedometer_screen->act_measured_data, 1) > 0)
    {
        if(speedometer_screen->act_measured_data != speedometer_screen->last_measured_data)
        {
            ssd1306_clear_screen();

            __itoa(speedometer_screen->act_measured_data, screen_buf, 10);

            ssd1306_draw_string(SPEEDOMETER_STRING_AREA_X, SPEEDOMETER_STRING_AREA_Y, screen_buf);
        }
    }

    /* drawing bitmap */
    ssd1306_draw_bitmap(SPEEDOMETER_BITMAP_AREA_X, SPEEDOMETER_BITMAP_AREA_Y, SPEEDOMETER_BITMAP_AREA_WIDTH, SPEEDOMETER_BITMAP_AREA_HEIGHT, speedometer_bitmap);
    ssd1306_draw_bitmap(MPH_BITMAP_AREA_X, MPH_BITMAP_AREA_Y, MPH_BITMAP_AREA_WIDTH, MPH_BITMAP_AREA_HEIGHT, mph_bitmap);

    speedometer_screen->last_measured_data = speedometer_screen->act_measured_data;
}

void display_battery_screen(display_screen_s_t* battery_screen)
{
    battery_screen->act_measured_data = adc_to_value(ADC_V_BAT, 0, 100);

    if(battery_screen->act_measured_data != battery_screen->last_measured_data)
    {
        ssd1306_clear_screen();

        __itoa(battery_screen->act_measured_data, screen_buf, 10);

        strcat(screen_buf, "%");

        ssd1306_draw_string(BATTERY_STRING_AREA_X, BATTERY_STRING_AREA_Y, screen_buf);
    }

    /* drawing bitmap */
    ssd1306_draw_bitmap(BATTERY_BITMAP_AREA_X, BATTERY_BITMAP_AREA_Y, BATTERY_BITMAP_AREA_WIDTH, BATTERY_BITMAP_AREA_HEIGHT, battery_bitmap);

    battery_screen->last_measured_data = battery_screen->act_measured_data;
}