/**
 * @file string_utils.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief string function
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "platform_specific.h"

/**
 * @defgroup utils_priority
 *
 * @{
 */

/**
 * @brief Conversion uint8_t value into char array with '\0' at the end
 * 
 * @param val 
 * @param c 
 * @return int32_t - Error code
 */
int32_t itoa(uint8_t val, uint8_t c[]);

/**
 * @}
 */

#endif /* PRIORITY_H */
