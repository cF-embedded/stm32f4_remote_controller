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
#include "platform_specific.h"
#include <stdlib.h>

/* Control buffer len -
   0 - "A" - Angle or "S" Speed
   1 -  "-" - Negative or "+" Positivie
   3 - Value from params struct range   */
#define CONTROL_BUFF_LEN 3

typedef struct
{
    const int32_t MAX_SPEED;
    const int32_t MIN_SPEED;
} speed_params_s_t;

typedef struct
{
    const int32_t MAX_ANGLE;
    const int32_t MIN_ANGLE;
} angle_params_s_t;

static speed_params_s_t speed_params = {.MAX_SPEED = 200, .MIN_SPEED = -200};

static angle_params_s_t angle_params = {.MAX_ANGLE = 90, .MIN_ANGLE = -90};

/**
 * @brief function convert adc val to speed params range
 *
 * @param adc_id
 * @return int32_t Measure speed value
 */
static int32_t joystick_control_adc2speed(uint8_t adc_id, speed_params_s_t _speed_params);

/**
 * @brief function convert adc val to angle params range
 *
 * @param adc_id
 * @return int32_t - Measure angle value
 */
static int32_t joystick_control_adc2angle(uint8_t adc_id, angle_params_s_t _angle_params);

/**
 * @brief
 *
 * @param val speed or angle value
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

void joystick_control_task_init(void)
{
    adc_init();

    rtos_task_create(joystick_control_send_speed, "joystick_send_speed", JOYSTICK_CONTROLLER_SPEED_STACKSIZE, JOYSTICK_CONTROLLER_SPEED_PRIORITY, NULL);
    rtos_task_create(joystick_control_send_angle, "joystick_send_angle", JOYSTICK_CONTROLLER_ANGLE_STACKSIZE, JOYSTICK_CONTROLLER_ANGLE_PRIORITY, NULL);
}

static int32_t joystick_control_adc2speed(uint8_t adc_id, speed_params_s_t _speed_params)
{
    int32_t adc_val = adc_val_get(adc_id);

    int32_t adc2speed = ((adc_val - ADC_MIN_VAL) * (_speed_params.MAX_SPEED - _speed_params.MIN_SPEED) / (ADC_MAX_VAL - ADC_MIN_VAL)) + _speed_params.MIN_SPEED;

    return adc2speed;
}

static int32_t joystick_control_adc2angle(uint8_t adc_id, angle_params_s_t _angle_params)
{
    int32_t adc_val = adc_val_get(adc_id);

    int32_t adc2angle = ((adc_val - ADC_MIN_VAL) * (_angle_params.MAX_ANGLE - _angle_params.MIN_ANGLE) / (ADC_MAX_VAL - ADC_MIN_VAL)) + _angle_params.MIN_ANGLE;

    return adc2angle;
}

uint8_t joystick_control_get_sign_from_val(int32_t val)
{
    return (val >= 0) ? '+' : '-';
}

static void joystick_control_send_speed(void* params)
{
    (void)params;

    uint8_t speed_control_buffer[CONTROL_BUFF_LEN] = {'S', 0, 0};
    int32_t speed_val;

    tick_t tick_cnt;

    while(1)
    {
        tick_cnt = rtos_tick_count_get();
        speed_val = joystick_control_adc2speed(ADC_SPEED_CONTROLLER, speed_params);

        speed_control_buffer[1] = joystick_control_get_sign_from_val(speed_val);
        speed_control_buffer[2] = abs(speed_val);

        // hm_10_send_buf(speed_control_buffer, CONTROL_BUFF_LEN);
        rtos_delay_until(&tick_cnt, 20);
    }
}

static void joystick_control_send_angle(void* params)
{
    (void)params;

    uint8_t angle_control_buffer[CONTROL_BUFF_LEN] = {'A', 0, 0};
    int32_t angle_val;

    tick_t tick_cnt;

    while(1)
    {
        tick_cnt = rtos_tick_count_get();
        angle_val = joystick_control_adc2angle(ADC_ANGLE_CONTROLLER, angle_params);

        angle_control_buffer[1] = joystick_control_get_sign_from_val(angle_val);
        angle_control_buffer[2] = abs(angle_val);

        // hm_10_send_buf(angle_control_buffer, CONTROL_BUFF_LEN);
        rtos_delay_until(&tick_cnt, 10);
    }
}