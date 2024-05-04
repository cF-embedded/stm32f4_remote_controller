/**
 * @file usart.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief USART periphal driver with FreeRTOS
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * file:	usart.h
 * author:	GAndaLF
 * brief:	USART peripheral driver.
 */

#ifndef _USART_H_
#define _USART_H_

#include "platform_specific.h"

/**
 * Initialize USART driver to work.
 *
 * This function initializes Tx to work with DMA and Rx to work with polling.
 */
void usart_init(void);

/**
 * Send buffer through USART.
 *
 * @param buf           Buffer to send.
 * @param len           Length of buffer.
 *
 * @return              Error code.
 */
int32_t usart_send_buf(uint8_t* buf, const int32_t len);

/**
 * Read buffer from USART.
 *
 * @param buf          Buffer to read.
 * @param len          Length of buffer.
 *
 * @return int32_t     Error code or Bytes Read amount.
 */
int32_t usart_read_buf(uint8_t* buf, const int32_t len);

#endif /* _USART_H_ */
