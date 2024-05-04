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
#include "usart.h"
#include <string.h>

/* AT Command buf length */
#define MAX_AT_COMMAND_LEN 32

/* Buffer to contain AT Commands for HM-10 Initialization */
static char at_command_buf[MAX_AT_COMMAND_LEN];

typedef struct
{
    uint8_t response[MAX_AT_COMMAND_LEN];
    uint8_t para1[MAX_AT_COMMAND_LEN];
} at_command_data_s_t;

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
        strcat(at_command_buf, command);
    }

    strcat(at_command_buf, "\r\n");

    size_t len = strlen(at_command_buf);

    return usart_send_buf((uint8_t*)at_command_buf, len);
}

static void hm_10_at_init_task(void* params)
{
    (void)params;

    at_command_data_s_t at_command_params;

    while(1)
    {
        hm_10_send_at_command(NULL);
        rtos_delay(2000);
    }

    vTaskSuspend(NULL);
}
