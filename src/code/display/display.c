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
#include "hm_10.h"
#include "i2c_master.h"
#include "platform_specific.h"
#include "speedometer_bitmap.h"
#include "ssd1306.h"

static TaskHandle_t display_handle;

typedef enum
{
    SPEEDOMETER_SCREEN = 0,
    BATTERY_SCREEN = 1
} display_screen_e_t;

/**
 * @brief show speedometer screen on display
 *
 */
static void display_show_speedometer_screen(void);
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

void display_show_speedometer_screen(void)
{
    static uint8_t speed = 0;

    ssd1306_draw_bitmap(SPEEDOMETER_BITMAP_AREA_X, SPEEDOMETER_BITMAP_AREA_Y, SPEEDOMETER_BITMAP_AREA_WIDTH, SPEEDOMETER_BITMAP_AREA_HEIGHT, speedometer_bitmap);

    ssd1306_draw_bitmap(MPH_BITMAP_AREA_X, MPH_BITMAP_AREA_Y, MPH_BITMAP_AREA_WIDTH, MPH_BITMAP_AREA_HEIGHT, mph_bitmap);

    hm_10_read_buf(&speed, 1);

    ssd1306_draw_string(SPEEDOMETER_STRING_AREA_X, SPEEDOMETER_STRING_AREA_Y, "200");

    ssd1306_update_screen();
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

    /* Suspend display task befor ssd1306 initialize */
    vTaskSuspend(NULL);
    while(1)
    {
        ticks = rtos_tick_count_get();

        switch(act_screen)
        {
            case SPEEDOMETER_SCREEN:
            {
                display_show_speedometer_screen();
                break;
            }

            case BATTERY_SCREEN:
            {
                break;
            }
        }

        rtos_delay_until(&ticks, 100);
    }
}
