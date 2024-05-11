/**
 * @file display_state_machine.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "display_state_machine.h"
#include "button.h"
#include "display_battery_bitmap.h"
#include "display_speedometer_bitmap.h"
#include "ssd1306.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


/* display screen string buff size  */
#define DISPLAY_STR_BUF_SIZE 4

typedef enum
{
    SPEEDOMETER_SCREEN = 0,
    BATTERY_SCREEN,
    MAX_SCREENS
} display_screen_e_t;

typedef struct
{
    bool is_bitmap_printed;
    uint8_t act_measured_data[DISPLAY_STR_BUF_SIZE];
    uint8_t last_measured_data[DISPLAY_STR_BUF_SIZE];
} display_screen_s_t;

typedef void (*display_callback_t)(const display_screen_s_t*);

typedef struct
{
    display_callback_t screen_func;
    display_screen_s_t* screen_data;
} display_entry_t;

/* current display screen for processing state machine */
static display_screen_e_t current_screen;

/* struct with data for speedometer screen */
static display_screen_s_t speedometer_screen;

/* struct with data for battery screen */
static display_screen_s_t battery_screen;

/**
 * @brief show speedometer screen on display
 *
 * @param speedometer_screen
 */
static void display_speedometer_screen(display_screen_s_t* speedometer_screen);

/**
 * @brief show battery screen on display
 *
 * @param battery_screen
 */
static void display_battery_screen(display_screen_s_t* battery_screen);

/**
 * @brief Check the screen swap button
 *
 * @return uint32_t - 1 - Pressed , 0 - Realeased
 */
static uint32_t display_is_button_pressed(void);

/**
 * @brief Set next screen of display
 *
 */
static void display_set_next_screen(void);

/**
 * @brief Lookup table with screens callbacks and data structs
 *
 */
static display_entry_t screen_table[MAX_SCREENS] = {{display_speedometer_screen, &speedometer_screen}, {display_battery_screen, &battery_screen}};

static void display_speedometer_screen(display_screen_s_t* speedometer_screen)
{
    if(!speedometer_screen->is_bitmap_printed)
    {
        ssd1306_draw_bitmap(SPEEDOMETER_BITMAP_AREA_X, SPEEDOMETER_BITMAP_AREA_Y, SPEEDOMETER_BITMAP_AREA_WIDTH, SPEEDOMETER_BITMAP_AREA_HEIGHT, speedometer_bitmap);
        ssd1306_draw_bitmap(MPH_BITMAP_AREA_X, MPH_BITMAP_AREA_Y, MPH_BITMAP_AREA_WIDTH, MPH_BITMAP_AREA_HEIGHT, mph_bitmap);

        speedometer_screen->is_bitmap_printed = true;
    }

    if(hm_10_read_buf(speedometer_screen->act_measured_data, DISPLAY_STR_BUF_SIZE) > 0)
    {
        if(strcmp(speedometer_screen->act_measured_data, speedometer_screen->last_measured_data) != 0)
        {
            // ssd1306_clear_screen();
            ssd1306_draw_string(SPEEDOMETER_STRING_AREA_X, SPEEDOMETER_STRING_AREA_Y, (char*)speedometer_screen->act_measured_data);
        }
    }

    strcpy(speedometer_screen->last_measured_data, speedometer_screen->act_measured_data);
}

static void display_battery_screen(display_screen_s_t* battery_screen)
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

static uint32_t display_is_button_pressed(void)
{
    return button_1_get();
}

static void display_set_next_screen(void)
{
    current_screen++;

    if(current_screen == MAX_SCREENS)
    {
        current_screen = SPEEDOMETER_SCREEN;
    }
}

void display_state_machine_init(void)
{
    /* first screen after start display */
    current_screen = SPEEDOMETER_SCREEN;
    /* clear screens data structs */
    memset(&speedometer_screen, 0, sizeof(display_screen_s_t));

    memset(&battery_screen, 0, sizeof(display_screen_s_t));
}

void display_state_machine_start(void)
{
    if((current_screen >= 0) && (current_screen < MAX_SCREENS))
    {
        screen_table[current_screen].screen_func(screen_table[current_screen].screen_data);

        // if(display_is_button_pressed())
        // {
        //     screen_table[current_screen].screen_data->is_bitmap_printed = false;
        //     display_set_next_screen();
        // }
    }

    /* always update screen after drawing */
    ssd1306_update_screen();
}