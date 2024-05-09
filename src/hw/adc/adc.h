/**
 * @file adc.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * file:	adc.h
 * author:	GAndaLF
 * brief:	ADC peripheral configuration.
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "platform_specific.h"

/* Order is dependent on channel sequence programmed in ADC */

/** Speed controller id. */
#define ADC_SPEED_CONTROLLER 0
/** Angle controller id. */
#define ADC_ANGLE_CONTROLLER 1
/** Battery monitor id. */
#define ADC_V_BAT 2

/** Maximum allowed ADC digital value. */
#define ADC_MAX_VAL 0x0FFFUL
/** Minimum allowed ADC digital value. */
#define ADC_MIN_VAL 0x0UL
/** Maximum allowed ADC voltage. */
#define ADC_MAX_VOLTAGE_MV 3300UL

/**
 * Initialisation of ADC peripheral.
 *
 * This function configures ADC to run on 3 channels in cyclic mode. Results are
 * stored in buffer using DMA.
 */
void adc_init(void);

/**
 * Get last ADC measurement for a given id.
 *
 * @param id                ADC id.
 *
 * @return                  ADC value.
 */
int32_t adc_val_get(int32_t id);

/**
 * @brief Convesrion adc to value in specific range
 *
 * @param id                ADC id.
 * @param max_val           Maximum value
 * @param min_val           Minimum value
 * @return int32_t          Value in specific range
 */
int32_t adc_to_value(int32_t id, int32_t max_val, int32_t min_val);

#endif /* _ADC_H_ */
