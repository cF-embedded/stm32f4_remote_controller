/**
 * @file hm_10.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief hm-10 driver with at commands
 * @date 2024-04-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "hm_10.h"
#include "hm_10_init_commands.h"
#include "joystick_controller.h"
#include "usart.h"
#include <string.h>

/* AT Command buf length */
#define MAX_AT_COMMAND_LEN 32

/* Time between At Commands in ms */
#define AT_COMMAND_DELAY 1000

/* Buffer to contain AT Commands for HM-10 Initialization */
static char at_command_buf[MAX_AT_COMMAND_LEN];

/**
 * @brief Initialization task with at commands of hm-10 module
 *
 * @param params Task parameteres - unused.
 */
static void hm_10_at_init_task(void* params);

/**
 * @brief Send at command to hm-10 module
 *
 * @return int32_t - Error code.
 */
static int32_t hm_10_send_at_command(const char* command);

void hm_10_task_init(void)
{
    usart_init();

    rtos_task_create(hm_10_at_init_task, "hm_10_at_init", HM_10_AT_INIT_TASK_STACKSIZE, HM_10_AT_INIT_TASK_PRIORITY, NULL);
}

static int32_t hm_10_send_at_command(const char* command)
{
    memset(at_command_buf, 0, MAX_AT_COMMAND_LEN);
    strcpy(at_command_buf, "AT");

    if(command != NULL)
    {
        strcat(at_command_buf, "+");
        strcat(at_command_buf, command);
    }

    strcat(at_command_buf, "\r\n");

    size_t len = strlen(at_command_buf);

    return hm_10_send_buf((uint8_t*)at_command_buf, len);
}

int32_t hm_10_send_buf(uint8_t* buf, const int32_t len)
{
    return usart_send_buf(buf, len);
}

int32_t hm_10_read_buf(uint8_t* buf, const int32_t len)
{
    return usart_read_buf(buf, len);
}

static void hm_10_at_init_task(void* params)
{
    (void)params;
    tick_t tick_cnt;

    for(uint8_t i = 0; i < AT_COMMANDS_INIT_SIZE; i++)
    {
        tick_cnt = rtos_tick_count_get();
        /* Send AT command init */
        hm_10_send_at_command(at_commands_init[i]);

        rtos_delay_until(&tick_cnt, AT_COMMAND_DELAY);
    }

    /* Resume joystick tasks after hm-10 initialization */
    vTaskResume(joystick_send_speed_handle);
    vTaskResume(joystick_send_angle_handle);

    vTaskSuspend(NULL);
}
