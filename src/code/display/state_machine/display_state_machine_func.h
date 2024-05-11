/**
 * @file display_state_machine_func.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief Screen functions for display state machine
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _DISPLAY_STATE_MACHINE_FUNC_H

#include "display_state_machine_types.h"

/**
 * @brief show speedometer screen on display
 *
 * @param speedometer_screen
 */
void display_speedometer_screen(display_screen_s_t* speedometer_screen);

/**
 * @brief show battery screen on display
 *
 * @param battery_screen
 */
void display_battery_screen(display_screen_s_t* battery_screen);

#endif /* _DISPLAY_STATE_MACHINE_FUNC_H */
