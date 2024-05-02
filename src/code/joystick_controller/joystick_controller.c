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

speed_params_s_t speed_params = {.MAX_SPEED = 200, .MIN_SPEED = -200};

angle_params_s_t angle_params = {.MAX_ANGLE = 180, .MIN_ANGLE = 0};

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

void joystick_control_task_init(void)
{
    adc_init();
}

static int32_t joystick_control_adc2speed(uint8_t adc_id, speed_params_s_t _speed_params)
{
    int32_t adc_val = adc_val_get(adc_id);
}

static int32_t joystick_control_adc2angle(uint8_t adc_id, angle_params_s_t _angle_params)
{
    int32_t adc_val = adc_val_get(adc_id);
}