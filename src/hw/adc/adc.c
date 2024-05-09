/**
 * @file adc.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief ADC peripheral configuration.
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * file:	adc.c
 * author:	GAndaLF
 * brief:   ADC peripheral configuration.
 */

#include "adc.h"
#include "gpio_f4.h"

/** Speed controller pin number - PC0. */
#define SPEED_CONTROLLER_PIN 0
/** Angle controller pin number - PC1. */
#define ANGLE_CONTROLLER_PIN 1
/** Battery monitor pin number -  PC2. */
#define V_BAT_PIN 2

/** Speed controller ADC channel. */
#define SPEED_CONTROLLER_CHANNEL 10
/** Angle controller ADC channel. */
#define ANGLE_CONTROLLER_CHANNEL 11
/** Battery monitor ADC channel. */
#define V_BAT_CHANNEL 12

/** DMA stream used for ADC. */
#define ADC_DMA DMA2_Stream0

/** Value of PRIO field in DMA CR register - Priority low. */
#define DMA_PRIO_LOW 0
/** Value of PRIO field in DMA CR register - Priority medium. */
#define DMA_PRIO_MEDIUM 1
/** Value of PRIO field in DMA CR register - Priority high. */
#define DMA_PRIO_HIGH 2
/** Value of PRIO field in DMA CR register - Priority very high. */
#define DMA_PRIO_VERYHIGH 3

/** Value of PSIZE and MSIZE fields in DMA CR register - Size 8-bit. */
#define DMA_SIZE_8BIT 0
/** Value of PSIZE and MSIZE fields in DMA CR register - Size 16-bit. */
#define DMA_SIZE_16BIT 1
/** Value of PSIZE and MSIZE fields in DMA CR register - Size 32-bit. */
#define DMA_SIZE_32BIT 2

/** Value of DIR filed in DMA CR register - Direction peripheral to memory. */
#define DMA_DIR_PERIPH_TO_MEM 0
/** Value of DIR filed in DMA CR register - Direction memory to peripheral. */
#define DMA_DIR_MEM_TO_PERIPH 1
/** Value of DIR filed in DMA CR register - Direction memory to memory. */
#define DMA_DIR_MEM_TO_MEM 2

/** Offset of bitfield CHSEL in DMA CR register. */
#define DMA_CR_CHSEL_BIT 25
/** Offset of bitfield PL in DMA CR register. */
#define DMA_CR_PL_BIT 16
/** Offset of bitfield MSIZE in DMA CR register. */
#define DMA_CR_MSIZE_BIT 13
/** Offset of bitfield PSIZE in DMA CR register. */
#define DMA_CR_PSIZE_BIT 11
/** Offset of bitfield DIR in DMA CR register. */
#define DMA_CR_DIR_BIT 6

/** Sample time 144 cycles of ADCCLK. */
#define ADC_SMP_144_CYCLES 6

/** Offset of bitfield SMP10 in ADC_SPMR2 register. */
#define ADC_SMPR1_SMP10_BIT 0
/** Offset of bitfield SMP8 in ADC_SPMR2 register. */
#define ADC_SMPR1_SMP11_BIT 3
/** Offset of bitfield SMP9 in ADC_SPMR2 register. */
#define ADC_SMPR1_SMP12_BIT 6

/** Offset of bitfield LEN in ADC_SQR1 register. */
#define ADC_SQR1_LEN_BIT 20

/** Offset of bitfield SQ7 in ADC_SQR2 register. */
#define ADC_SQR2_SQ7_BIT 0

/** Offset of bitfield SQ1 in ADC_SQR3 register. */
#define ADC_SQR3_SQ1_BIT 0
/** Offset of bitfield SQ2 in ADC_SQR3 register. */
#define ADC_SQR3_SQ2_BIT 5
/** Offset of bitfield SQ3 in ADC_SQR3 register. */
#define ADC_SQR3_SQ3_BIT 10
/** Offset of bitfield SQ4 in ADC_SQR3 register. */
#define ADC_SQR3_SQ4_BIT 15
/** Offset of bitfield SQ5 in ADC_SQR3 register. */
#define ADC_SQR3_SQ5_BIT 20
/** Offset of bitfield SQ6 in ADC_SQR3 register. */
#define ADC_SQR3_SQ6_BIT 25

/** Number of ADC measurements in a cycle. */
#define ADC_BUF_SIZE 3

/**
 * Array storing actual measured ADC values.
 */
static int32_t adc_buf[ADC_BUF_SIZE];

/**
 * GPIO initialization for ADC.
 */
static void gpio_init(void);

/**
 * DMA configuration to run with ADC.
 */
static void dma_init(void);

/**
 * ADC1 peripheral initialization.
 */
static void adc1_init(void);

void adc_init(void)
{
    gpio_init();
    dma_init();
    adc1_init();
}

int32_t adc_val_get(int32_t id)
{
    if(id < 0 || id >= ADC_BUF_SIZE)
    {
        return -EINVAL;
    }

    return adc_buf[id];
}

int32_t adc_to_value(int32_t id, int32_t max_val, int32_t min_val)
{
    int32_t adc_val, specific_val, adc_val_diff, specific_val_diff = {0};

    adc_val = adc_val_get(id);

    adc_val_diff = (ADC_MAX_VAL - ADC_MIN_VAL);
    specific_val_diff = (max_val - min_val);

    specific_val = ((adc_val - ADC_MIN_VAL) * specific_val_diff / adc_val_diff) + min_val;

    return specific_val;
}

static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    gpio_mode_config(GPIOC, SPEED_CONTROLLER_PIN, GPIO_MODE_ANALOG);
    gpio_mode_config(GPIOC, ANGLE_CONTROLLER_PIN, GPIO_MODE_ANALOG);
    gpio_mode_config(GPIOC, V_BAT_PIN, GPIO_MODE_ANALOG);
}

static void dma_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    ADC_DMA->CR = (0 << DMA_CR_CHSEL_BIT) |        /* Set channel 0 */
        (DMA_PRIO_MEDIUM << DMA_CR_PL_BIT) |       /* Set priority */
        (DMA_SIZE_32BIT << DMA_CR_MSIZE_BIT) |     /* Set memory size */
        (DMA_SIZE_32BIT << DMA_CR_PSIZE_BIT) |     /* Set periph size */
        DMA_SxCR_MINC |                            /* Set memory increment */
        DMA_SxCR_CIRC |                            /* Set circular mode */
        (DMA_DIR_PERIPH_TO_MEM << DMA_CR_DIR_BIT); /* Set direction */

    /* Set source, destination and size */
    ADC_DMA->NDTR = ADC_BUF_SIZE;
    ADC_DMA->PAR = (uint32_t)&ADC1->DR;
    ADC_DMA->M0AR = (uint32_t)adc_buf;

    /* Enable DMA channel */
    ADC_DMA->CR |= DMA_SxCR_EN;
}

static void adc1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* Max allowed ADC frequency - 36MHz */
    ADC->CCR = 0; /* Set prescaler to 2 which gives 84 /2 /2 = 21MHz */

    ADC1->CR1 = ADC_CR1_SCAN; /* Enable scan mode */
    ADC1->CR2 = ADC_CR2_DMA | /* Enable DMA */
        ADC_CR2_DDS |         /* DMA dont stop after first conversion */
        ADC_CR2_CONT;         /* Enable continuous conversion */

    /* Set sampling time to 144 cycles for channels 10 - 12 */
    ADC1->SMPR1 = (ADC_SMP_144_CYCLES << ADC_SMPR1_SMP10_BIT) | (ADC_SMP_144_CYCLES << ADC_SMPR1_SMP11_BIT) | (ADC_SMP_144_CYCLES << ADC_SMPR1_SMP12_BIT);

    /* Set 3 conversions on channels 0 - 2 */
    ADC1->SQR1 = ((ADC_BUF_SIZE - 1) << ADC_SQR1_LEN_BIT);
    ADC1->SQR3 = (SPEED_CONTROLLER_CHANNEL << ADC_SQR3_SQ1_BIT) | (ANGLE_CONTROLLER_CHANNEL << ADC_SQR3_SQ2_BIT) | (V_BAT_CHANNEL << ADC_SQR3_SQ3_BIT);

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}
