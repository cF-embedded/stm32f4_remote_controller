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
#include "button.h"
#include "hm_10.h"
#include "i2c_master.h"
#include "platform_specific.h"
#include "ssd1306.h"

/* handle for dispaly task */
static TaskHandle_t display_handle;

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
    button_init();
    display_state_machine_init();

    rtos_task_create(ssd1306_init_task, "ssd1306_init", SSD1306_INIT_STACKSIZE, SSD1306_INIT_PRIORITY, NULL);
    rtos_task_create(display_task, "display", DISPLAY_STACKSIZE, DISPLAY_PRIORITY, &display_handle);
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

    /* Suspend display task befor ssd1306 initialize */
    vTaskSuspend(NULL);
    while(1)
    {
        ticks = rtos_tick_count_get();

        display_state_machine_start();

        rtos_delay_until(&ticks, 100);
    }
}
