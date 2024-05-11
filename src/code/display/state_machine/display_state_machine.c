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
#include "display_state_machine_func.h"
#include "display_state_machine_types.h"
#include "ssd1306.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* current display screen for processing state machine */
static display_screen_e_t current_screen;

/* struct with data for speedometer screen */
static display_screen_s_t speedometer_screen;

/* struct with data for battery screen */
static display_screen_s_t battery_screen;

/**
 * @brief Lookup table with screens callbacks and data structs
 *
 */
static display_entry_t screen_table[MAX_SCREENS] = {{display_speedometer_screen, &speedometer_screen}, {display_battery_screen, &battery_screen}};

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
    }

    if(display_is_button_pressed())
    {
        screen_table[current_screen].screen_data->is_bitmap_printed = false;

        // ssd1306_clear_screen();

        display_set_next_screen();
    }

    /* always update screen after drawing */
    ssd1306_update_screen();
}