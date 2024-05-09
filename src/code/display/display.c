/**
 * @file display.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "display.h"
#include "battery_bitmap.h"
#include "button.h"
#include "hm_10.h"
#include "i2c_master.h"
#include "platform_specific.h"
#include "speedometer_bitmap.h"
#include "ssd1306.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* handle for dispaly task */
static TaskHandle_t display_handle;

/* display screen string buff size  */
#define DISPLAY_STR_BUF_SIZE 4

typedef enum
{
    SPEEDOMETER_SCREEN = 0,
    BATTERY_SCREEN = 1
} display_screen_e_t;

typedef struct
{
    bool is_screen_printed;
    uint8_t act_measured_data[DISPLAY_STR_BUF_SIZE];
    uint8_t last_measured_data[DISPLAY_STR_BUF_SIZE];
} display_screen_s_t;

/**
 * @brief show speedometer screen on display
 *
 * @param speedometer_screen
 */
static void display_show_speedometer_screen(display_screen_s_t* speedometer_screen);

/**
 * @brief show battery screen on display
 *
 * @param battery_screen
 */
static void display_show_battery_screen(display_screen_s_t* battery_screen);

/**
 * @brief Display task handler.
 *
 * @param params Task parameters - unused.
 */
static void display_task(void* params);

/**
 * @brief ssd1306 initalization, deleted after init.
 *
 * @param params Task parameters - unused
 */
static void ssd1306_init_task(void* params);

void display_tasks_init(void)
{
    i2c_master_init();

    rtos_task_create(ssd1306_init_task, "ssd1306_init", SSD1306_INIT_STACKSIZE, SSD1306_INIT_PRIORITY, NULL);
    rtos_task_create(display_task, "display", DISPLAY_STACKSIZE, DISPLAY_PRIORITY, &display_handle);
}

void display_show_speedometer_screen(display_screen_s_t* speedometer_screen)
{
    if(!speedometer_screen->is_screen_printed)
    {
        ssd1306_draw_bitmap(SPEEDOMETER_BITMAP_AREA_X, SPEEDOMETER_BITMAP_AREA_Y, SPEEDOMETER_BITMAP_AREA_WIDTH, SPEEDOMETER_BITMAP_AREA_HEIGHT, speedometer_bitmap);
        ssd1306_draw_bitmap(MPH_BITMAP_AREA_X, MPH_BITMAP_AREA_Y, MPH_BITMAP_AREA_WIDTH, MPH_BITMAP_AREA_HEIGHT, mph_bitmap);

        speedometer_screen->is_screen_printed = true;
    }

    if(hm_10_read_buf(speedometer_screen->act_measured_data, DISPLAY_STR_BUF_SIZE) > 0)
    {
        if(strcmp(speedometer_screen->act_measured_data, speedometer_screen->last_measured_data) != 0)
        {
            ssd1306_clear_screen();
            ssd1306_draw_string(SPEEDOMETER_STRING_AREA_X, SPEEDOMETER_STRING_AREA_Y, (char*)speedometer_screen->act_measured_data);
        }
    }

    strcpy(speedometer_screen->last_measured_data, speedometer_screen->act_measured_data);
}

void display_show_battery_screen(display_screen_s_t* battery_screen)
{
    if(!battery_screen->is_screen_printed)
    {
        ssd1306_draw_bitmap(BATTERY_BITMAP_AREA_X, BATTERY_BITMAP_AREA_Y, BATTERY_BITMAP_AREA_WIDTH, BATTERY_BITMAP_AREA_HEIGHT, battery_bitmap);

        battery_screen->is_screen_printed = true;
    }

    char* vbat_str[5];

    float vbat = 2.9;

    // snprintf(vbat_str, sizeof(vbat_str), "%.1f", vbat);
    strcat(vbat_str, "V");

    ssd1306_draw_string(BATTERY_STRING_AREA_X, BATTERY_STRING_AREA_Y, vbat_str);
}

static void ssd1306_init_task(void* params)
{
    (void)params;

    while(1)
    {
        ssd1306_init();
        /* Resume display task after ssd1306 init */
        vTaskResume(display_handle);
        /* Suspend ssd1306_init_task */
        vTaskSuspend(NULL);
    }
}

static void display_task(void* params)
{
    (void)params;

    tick_t ticks;

    display_screen_e_t act_screen = SPEEDOMETER_SCREEN;

    display_screen_s_t speedometer_screen, battery_screen;

    memset(&speedometer_screen, 0, sizeof(display_screen_s_t));
    memset(&battery_screen, 0, sizeof(display_screen_s_t));

    /* Suspend display task befor ssd1306 initialize */
    vTaskSuspend(NULL);
    while(1)
    {
        ticks = rtos_tick_count_get();

        switch(act_screen)
        {
            case SPEEDOMETER_SCREEN:
            {
                battery_screen.is_screen_printed = false;

                display_show_speedometer_screen(&speedometer_screen);
                break;
            }

            case BATTERY_SCREEN:
            {
                speedometer_screen.is_screen_printed = false;

                display_show_battery_screen(&battery_screen);
                break;
            }
        }

        ssd1306_update_screen();
        rtos_delay_until(&ticks, 100);
    }
}
