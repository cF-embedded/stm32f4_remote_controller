/**
 * @file display_state_machine_types.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief types for display state machine
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _DISPLAY_STATE_MACHINE_TYPES_H

#include "platform_specific.h"

#define DISPLAY_STR_BUF_SIZE 4

/* possible display screens */
typedef enum
{
    SPEEDOMETER_SCREEN = 0,
    BATTERY_SCREEN,
    MAX_SCREENS
} display_screen_e_t;

/* struct with display data*/
typedef struct
{
    uint16_t act_measured_data;
    uint16_t last_measured_data;
} display_screen_s_t;

/* display screen callback */
typedef void (*display_callback_t)(display_screen_s_t*);

/* struct for Lookup Table with func and data */
typedef struct
{
    display_callback_t screen_func;
    display_screen_s_t* screen_data;
} display_entry_t;

#define _DISPLAY_STATE_MACHINE_TYPES_H

#endif /* _DISPLAY_STATE_MACHINE_TYPES_H */
