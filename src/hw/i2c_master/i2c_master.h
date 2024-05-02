/**
 * @file i2c_master.h
 * @author cf-embedded.pl
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _I2C_MASTER_H_
#define _I2C_MASTER_H_

#include "platform_specific.h"

/**
 * @brief initialization of i2c periphal as master
 *
 */
void i2c_master_init(void);

/**
 * @brief i2c function
 *
 * @param data
 * @param slave_addr
 * @param n_bytes
 * @return int32_t
 */
int32_t i2c_master_write(uint8_t* data, uint8_t slave_addr, int32_t n_bytes);

#endif /* _I2C_MASTER_H_ */
