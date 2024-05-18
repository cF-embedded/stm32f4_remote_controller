/**
 * @file priority.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief tasks and interrupts priorities
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef PRIORITY_H
#define PRIORITY_H

/**
 * @defgroup utils_priority
 *
 * @{
 */

/** Position estimation task stacksize */
#define HM_10_AT_INIT_TASK_STACKSIZE (configMINIMAL_STACK_SIZE * 4)
/** Position estimation task priority */
#define HM_10_AT_INIT_TASK_PRIORITY (tskIDLE_PRIORITY + 8)

#define JOYSTICK_CONTROLLER_STACKSIZE (configMINIMAL_STACK_SIZE * 2)
/** Position estimation task priority */
#define JOYSTICK_CONTROLLER_PRIORITY (tskIDLE_PRIORITY + 8)

/** SSD1306 initialization stacksize */
#define SSD1306_INIT_STACKSIZE (configMINIMAL_STACK_SIZE * 2)
/** SSD1306 initialization priority */
#define SSD1306_INIT_PRIORITY (tskIDLE_PRIORITY + 10)

/** Display batter stacksize */
#define DISPLAY_STACKSIZE (configMINIMAL_STACK_SIZE * 10)
/** Display battery priority */
#define DISPLAY_PRIORITY (tskIDLE_PRIORITY + 5)

/* USART HW priority */
#define USART_PRIORITY 7
/** DMA on I2C TX HW priority */
#define DMA_I2C_TX_PRIORITY 8
/** I2C1 Event HW priority */
#define I2C1_EV_PRIORITY 8
/** I2C1 Event HW priority */
#define EXTI_PRIORITY 9

/**
 * @}
 */

#endif /* PRIORITY_H */
