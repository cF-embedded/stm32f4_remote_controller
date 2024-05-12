/**
 * @file joystick_controller.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief Speed and angle control by joystick
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _JOSYSTICK_CONTROL_H_
#define _JOSYSTICK_CONTROL_H_

#include "platform_specific.h"

/* Handle for joystick send measure task */
extern TaskHandle_t joystick_send_measure_handle;

/**
 * @brief Initalization joystick speed and angle control task
 *
 */
void joystick_control_task_init(void);

#endif /* _JOSYSTICK_CONTROL_H_ */
