/**
 * @file display_state_machine_func.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief Screen functions for display state machine
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "display_battery_bitmap.h"
#include "display_speedometer_bitmap.h"
#include "display_state_machine_types.h"
#include "hm_10.h"
#include "ssd1306.h"
#include <string.h>

void display_speedometer_screen(display_screen_s_t* speedometer_screen)
{
    if(!speedometer_screen->is_bitmap_printed)
    {
        ssd1306_draw_bitmap(SPEEDOMETER_BITMAP_AREA_X, SPEEDOMETER_BITMAP_AREA_Y, SPEEDOMETER_BITMAP_AREA_WIDTH, SPEEDOMETER_BITMAP_AREA_HEIGHT, speedometer_bitmap);
        ssd1306_draw_bitmap(MPH_BITMAP_AREA_X, MPH_BITMAP_AREA_Y, MPH_BITMAP_AREA_WIDTH, MPH_BITMAP_AREA_HEIGHT, mph_bitmap);

        speedometer_screen->is_bitmap_printed = true;
    }

    // if(hm_10_read_buf(speedometer_screen->act_measured_data, DISPLAY_STR_BUF_SIZE) > 0)
    // {
    //     if(strcmp(speedometer_screen->act_measured_data, speedometer_screen->last_measured_data) != 0)
    //     {
    //         // ssd1306_clear_screen();
    //         ssd1306_draw_string(SPEEDOMETER_STRING_AREA_X, SPEEDOMETER_STRING_AREA_Y, (char*)speedometer_screen->act_measured_data);
    //     }
    // }

    strcpy(speedometer_screen->last_measured_data, speedometer_screen->act_measured_data);
}

void display_battery_screen(display_screen_s_t* battery_screen)
{
    if(!battery_screen->is_bitmap_printed)
    {
        ssd1306_draw_bitmap(BATTERY_BITMAP_AREA_X, BATTERY_BITMAP_AREA_Y, BATTERY_BITMAP_AREA_WIDTH, BATTERY_BITMAP_AREA_HEIGHT, battery_bitmap);

        battery_screen->is_bitmap_printed = true;
    }

    char* vbat_str = "2.8";

    float vbat = 2.9;

    // snprintf(vbat_str, sizeof(vbat_str), "%.1f", vbat);
    strcat(vbat_str, "V");

    ssd1306_draw_string(BATTERY_STRING_AREA_X, BATTERY_STRING_AREA_Y, vbat_str);
}