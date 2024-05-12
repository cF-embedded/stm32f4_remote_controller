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
static void joystick_control_send_speed(void* params);

/**
 * @brief Task angle send to rc car
 *
 * @param params - unused
 */
static void joystick_control_send_angle(void* params);

/**
 * @brief Set controll buffer to send
 *
 * @param buff - to send
 * @param val - measured adc value
 * @return int16_t - Error code
 */
static int16_t joystick_set_control_buff(uint8_t* buff, int16_t val);

void joystick_control_task_init(void)
{
    adc_init();

    rtos_task_create(joystick_control_send_speed, "joystick_send_speed", JOYSTICK_CONTROLLER_SPEED_STACKSIZE, JOYSTICK_CONTROLLER_SPEED_PRIORITY, &joystick_send_speed_handle);
    rtos_task_create(joystick_control_send_angle, "joystick_send_angle", JOYSTICK_CONTROLLER_ANGLE_STACKSIZE, JOYSTICK_CONTROLLER_ANGLE_PRIORITY, &joystick_send_angle_handle);
}

static uint8_t joystick_control_get_sign_from_val(int32_t val)
{
    return (val >= 0) ? '+' : '-';
}

static int16_t joystick_set_control_buff(uint8_t* buff, int16_t val)
{
    if(buff == NULL)
    {
        return -EINVAL;
    }

    buff[CONTROL_BUFF_LEN - 2] = joystick_control_get_sign_from_val(val);
    buff[CONTROL_BUFF_LEN - 1] = abs(val);

    return 0;
}

static void joystick_control_send_speed(void* params)
{
    (void)params;

    uint8_t speed_control_buffer[CONTROL_BUFF_LEN] = {'S', 0, 0};
    int16_t speed_val;

    tick_t tick_cnt;

    vTaskSuspend(NULL);

    while(1)
    {
        tick_cnt = rtos_tick_count_get();

        speed_val = adc_to_value(ADC_SPEED_CONTROLLER, JOYSTICK_SPEED_MAX, -JOYSTICK_SPEED_MAX);

        if(joystick_set_control_buff(speed_control_buffer, speed_val) == 0)
        {
            hm_10_send_buf(speed_control_buffer, CONTROL_BUFF_LEN);
        }

        rtos_delay_until(&tick_cnt, 15);
    }
}

static void joystick_control_send_angle(void* params)
{
    (void)params;

    uint8_t angle_control_buffer[CONTROL_BUFF_LEN] = {'A', 0, 0};
    int32_t angle_val;

    tick_t tick_cnt;

    vTaskSuspend(NULL);

    while(1)
    {
        tick_cnt = rtos_tick_count_get();
        angle_val = adc_to_value(ADC_ANGLE_CONTROLLER, JOYSTICK_ANGLE_MAX, -JOYSTICK_ANGLE_MAX);

        if(joystick_set_control_buff(angle_control_buffer, angle_val) == 0)
        {
            hm_10_send_buf(angle_control_buffer, CONTROL_BUFF_LEN);
        }

        rtos_delay_until(&tick_cnt, 10);
    }
}
