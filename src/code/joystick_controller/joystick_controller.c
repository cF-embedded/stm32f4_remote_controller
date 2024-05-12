/**
 * @file joystick_controller.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief Speed and angle control by joystick
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "joystick_controller.h"
#include "adc.h"
#include "hm_10.h"
#include "platform_specific.h"
#include "string.h"
#include <stdlib.h>

/* Handle for joystick send measure task */
TaskHandle_t joystick_send_measure_handle;

/* Joystick measure value in percentages */
#define JOYSTICK_MEASURE_MAX 100

/* Len of control buffer */
#define CONTROL_BUF_LEN 3

/** "A" - Angle or "S" - Speed */
#define CONTROL_CHAR_INDEX 0
/** "-" - Negative or "+" - Positivie */
#define CONTROL_SIGN_INDEX 1
/** Measured value in percentages  */
#define CONTROL_VAL_INDEX 2

typedef struct
{
    uint8_t control_char;
    uint8_t adc_id;
} joystick_control_buf_s_t;

/* Joystick table to contain control char for send message and id in adc buf */
static joystick_control_buf_s_t joystick_send_table[] = {{.control_char = 'S', .adc_id = ADC_SPEED_CONTROLLER}, {.control_char = 'A', .adc_id = ADC_ANGLE_CONTROLLER}};

/**
 * @brief get positive or negative char from value
 *
 * @param val
 * @return uint8_t '+' or '-'
 */
static uint8_t joystick_control_get_sign_from_val(int32_t val);

/**
 * @brief Task speed send to rc car
 *
 * @param Params - unused
 */
static void joystick_control_send_measure(void* params);

/**
 * @brief set control buffer characters before send
 *
 * @param buff - to send
 * @param val - measured value from adc
 * @param control_char - control char as first character
 * @return int16_t
 */
static int16_t joystick_set_control_buff(uint8_t* buff, int16_t val, uint8_t control_char);

void joystick_control_task_init(void)
{
    adc_init();

    rtos_task_create(joystick_control_send_measure, "joystick_send_measure", JOYSTICK_CONTROLLER_STACKSIZE, JOYSTICK_CONTROLLER_PRIORITY, &joystick_send_measure_handle);
}

static uint8_t joystick_control_get_sign_from_val(int32_t val)
{
    return (val >= 0) ? '+' : '-';
}

static int16_t joystick_set_control_buff(uint8_t* buf, int16_t val, uint8_t control_char)
{
    if(buf == NULL)
    {
        return -EINVAL;
    }

    buf[CONTROL_CHAR_INDEX] = control_char;
    buf[CONTROL_SIGN_INDEX] = joystick_control_get_sign_from_val(val);
    buf[CONTROL_VAL_INDEX] = abs(val);

    return 0;
}

static void joystick_control_send_measure(void* params)
{
    (void)params;

    uint8_t control_buf[CONTROL_BUF_LEN];
    int16_t send_val, send_table_size;

    tick_t tick_cnt;

    vTaskSuspend(NULL);

    send_table_size = (sizeof(joystick_send_table) / sizeof(joystick_send_table[0]));

    while(1)
    {
        for(uint8_t i = 0; i < send_table_size; i++)
        {
            tick_cnt = rtos_tick_count_get();

            send_val = adc_to_value(joystick_send_table[i].adc_id, JOYSTICK_MEASURE_MAX, -JOYSTICK_MEASURE_MAX);

            if(joystick_set_control_buff(control_buf, send_val, joystick_send_table[i].control_char) == 0)
            {
                hm_10_send_buf(control_buf, CONTROL_BUF_LEN);
            }

            rtos_delay_until(&tick_cnt, 5);
        }
    }
}
