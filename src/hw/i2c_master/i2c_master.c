/**
 * @file i2c_master.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief i2c master driver
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "i2c_master.h"
#include "gpio_f4.h"
#include "platform_specific.h"

/** DMA Channel 1 */
#define DMA_CHANNEL_1 1
/** DMA Channel 2 */
#define DMA_CHANNEL_2 2
/** DMA Channel 3 */
#define DMA_CHANNEL_3 3
/** DMA Channel 4 */
#define DMA_CHANNEL_4 4
/** DMA Channel 5 */
#define DMA_CHANNEL_5 5
/** DMA Channel 6 */
#define DMA_CHANNEL_6 6

/** I2C SCL pin number - PB06. */
#define I2C_SCL_PIN 6
/** O2C SDA pin number - PB07. */
#define I2C_SDA_PIN 7

/** DMA stream used for I2C Tx. */
#define I2C_TX_DMA DMA1_Stream6

/** Macro for calculating period in ns for a given frequency. */
#define PERIOD_NS(freq_hz) (1000000000ULL / (freq_hz))

/** Macro for calculating CCR value in 16/9 mode. */
#define I2C_CCR_16_9_VAL(i2c_freq, pb_freq) (PERIOD_NS(i2c_freq) / (25 * PERIOD_NS(pb_freq)))

/** Macro for calculating TRISE value. */
#define I2C_TRISE_VAL(i2c_freq, pb_freq) (((PERIOD_NS(i2c_freq) / 10) / PERIOD_NS(pb_freq)) + 1)

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

/**
 * Possible states of I2C driver
 */
enum i2c_state
{
    I2C_IDLE, /**< No transfer in progress */
    I2C_TX,   /**< Tx transfer in progress */
};

/**
 * Internal I2C driver parameters
 */
struct i2c_params
{
    uint8_t slave_addr;   /**< I2C slave address */
    enum i2c_state state; /**< Internal state of the I2C driver */
    void* sem;            /**< Semaphore guarding access to the peripheral */
};

/**
 * Internal I2C driver parameters.
 */
static struct i2c_params params;

/**
 * Initialization of GPIOs used by I2C.
 */
static void gpio_init(void);

/**
 * Initialization of the I2C peripheral.
 */
static void i2c_init(void);

/**
 * Initialization of DMA channels used by the I2C.
 */
static void dma_init(void);

/**
 * DMA request for sending data to I2C slave.
 *
 * @param data          Buffer with data to send.
 * @param n_bytes       Number of bytes to send.
 */
static void dma_request_tx(uint8_t* data, int32_t n_bytes);

void i2c_master_init(void)
{
    params.sem = rtos_sem_bin_create();
    rtos_sem_give(params.sem);
    params.state = I2C_IDLE;

    gpio_init();
    i2c_init();
    dma_init();
}

int32_t i2c_master_write(uint8_t* data, uint8_t slave_addr, int32_t n_bytes)
{
    if((data == NULL) || (n_bytes < 1))
    {
        /* Invalid arguments */
        return -EINVAL;
    }

    if(rtos_sem_take(params.sem, 10) != true)
    {
        /* Report timeout */
        return -EBUSY;
    }

    dma_request_tx(data, n_bytes);
    params.slave_addr = (slave_addr << 1);
    params.state = I2C_TX;

    /* Enable I2C event interrupt */
    I2C1->CR2 |= I2C_CR2_ITEVTEN;

    /* Send start signal */
    I2C1->CR1 |= I2C_CR1_START;

    if(rtos_sem_take(params.sem, 10) != true)
    {
        /* Report timeout */
        return -EBUSY;
    }

    rtos_sem_give(params.sem);
    params.state = I2C_IDLE;

    return 0;
}

static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    gpio_mode_config(GPIOB, I2C_SCL_PIN, GPIO_MODE_AF);
    gpio_mode_config(GPIOB, I2C_SDA_PIN, GPIO_MODE_AF);

    gpio_af_config(GPIOB, I2C_SCL_PIN, GPIO_AF_I2C1);
    gpio_af_config(GPIOB, I2C_SDA_PIN, GPIO_AF_I2C1);
}

static void i2c_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR2 = APB1_CLOCK_FREQ / 1000000UL;
    I2C1->CCR = I2C_CCR_FS | /* I2C Fast mode */
        I2C_CCR_DUTY |       /* Duty cycle 16/9 */
        I2C_CCR_16_9_VAL(400000UL, APB1_CLOCK_FREQ);
    I2C1->TRISE = I2C_TRISE_VAL(400000UL, APB1_CLOCK_FREQ);

    I2C1->CR1 = I2C_CR1_PE;
    I2C1->CR2 |= I2C_CR2_DMAEN;

    NVIC_SetPriority(I2C1_EV_IRQn, I2C1_EV_PRIORITY);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
}

static void dma_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    NVIC_SetPriority(DMA1_Stream6_IRQn, DMA_I2C_TX_PRIORITY);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

static void dma_request_tx(uint8_t* data, int32_t n_bytes)
{
    I2C_TX_DMA->CR = (1 << DMA_CR_CHSEL_BIT) |      /* Set channel 1 */
        (DMA_PRIO_HIGH << DMA_CR_PL_BIT) |          /* Priority set to high */
        (DMA_SIZE_8BIT << DMA_CR_MSIZE_BIT) |       /* Set memory size */
        (DMA_SIZE_8BIT << DMA_CR_PSIZE_BIT) |       /* Set peripheral size */
        DMA_SxCR_MINC |                             /* Memory increment */
        (DMA_DIR_MEM_TO_PERIPH << DMA_CR_DIR_BIT) | /* Set direction */
        DMA_SxCR_TCIE;                              /* Transfer complete IRQ */

    I2C_TX_DMA->PAR = (uint32_t)&I2C1->DR;
    I2C_TX_DMA->M0AR = (uint32_t)data;
    I2C_TX_DMA->NDTR = n_bytes;
}

void I2C1_EV_IRQHandler(void)
{
    int32_t yield = 0;
    uint32_t sr1;
    volatile uint32_t dummy;

    sr1 = I2C1->SR1;

    /* Check for the end of start event */
    if((sr1 & I2C_SR1_SB) != 0)
    {
        I2C1->DR = params.slave_addr;
    }
    else if((sr1 & I2C_SR1_ADDR) != 0)
    {
        /* Clear the ADDR flag by reading SR1 and SR2 registers */
        dummy = I2C1->SR1;
        dummy = I2C1->SR2;
        /* Prevent compiler warnings about unused variable */
        (void)dummy;
        /* Disable I2C1 event interrupt during DMA transfer */
        I2C1->CR2 &= ~I2C_CR2_ITEVTEN;

        /* Start TX DMA */
        I2C_TX_DMA->CR |= DMA_SxCR_EN;
    }
    else if((sr1 & I2C_SR1_BTF) != 0)
    {
        if(params.state == I2C_TX)
        {
            /* Disable I2C1 event interrupt as it is the end of the tx */
            I2C1->CR2 &= ~I2C_CR2_ITEVTEN;

            /* Send stop signal */
            I2C1->CR1 |= I2C_CR1_STOP;

            rtos_sem_give_isr(params.sem, &yield);
        }
        else
        {}
    }
    else
    {}

    portYIELD_FROM_ISR(yield);
}

void DMA1_Stream6_IRQHandler(void)
{
    if((DMA1->HISR & DMA_HISR_TCIF6) != 0)
    {
        /* Clear transfer complete flag and DMA enable flag */
        DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
        I2C_TX_DMA->CR &= ~(DMA_SxCR_EN);

        /* Enable I2C event interrupt to check for BTF */
        I2C1->CR2 |= I2C_CR2_ITEVTEN;
    }
}
