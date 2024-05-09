/**
 * @file button.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief Hardware driver for buttons.
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * file:	button.h
 * author:	GAndaLF
 * brief:	Hardware driver for buttons.
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "platform_specific.h"

/**
 * Initialise button pins to work.
 */
void button_init(void);

/**
 * Get state of button 1 pin.
 *
 * @return          Button state - 1 - released, 0 - pushed.
 */
uint32_t button_1_get(void);

#endif /* _BUTTON_H_ */