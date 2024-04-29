/**
 * @file usart.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief USART periphal drvier with FreeRTOS
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * file:	usart.c
 * author:	GAndaLF
 * brief:	USART peripheral driver.
 */

#include "usart.h"
#include "gpio_f4.h"
#include "platform_specific.h"

/* queue for send data by USART */
static queue_t tx_queue;
/* length of tx queue */
#define TX_QUEUE_LEN 32

/* queue for receive data from USART */
static queue_t rx_queue;
/* length of rx queue */
#define RX_QUEUE_LEN 32

/** USART baud rate. */
#define USART_BAUD_RATE 9600

/** Pin number used for USART TX - PA09. */
#define USART_TX_PIN 2
/** Pin number used for USART RX - PA10. */
#define USART_RX_PIN 3

/** Mutex serialising access to USART tx. */
static sem_t tx_sem_bin;

/** Mutex serialising access to USART rx. */
static sem_t rx_sem_bin;

/**
 * @brief Initialization of USART gpio
 *
 */
static void gpio_init(void);

/**
 * @brief Initialization of USART2
 *
 */
static void usart2_init(void);

void usart_init(void)
{
    tx_sem_bin = rtos_sem_bin_create();
    rx_sem_bin = rtos_sem_bin_create();
    rtos_sem_give(tx_sem_bin);
    rtos_sem_give(rx_sem_bin);

    tx_queue = rtos_queue_create(TX_QUEUE_LEN, sizeof(uint8_t));
    rx_queue = rtos_queue_create(RX_QUEUE_LEN, sizeof(uint8_t));

    gpio_init();
    usart2_init();
}

int32_t usart_send_buf(uint8_t* buf, const int32_t n_bytes)
{
    if((buf == NULL) || (n_bytes < 1))
    {
        /* Invalid arguments */
        return -EINVAL;
    }

    if(rtos_sem_take(tx_sem_bin, 10) != true)
    {
        /* Report timeout */
        return -EBUSY;
    }

    for(uint8_t i = 0; i < n_bytes; i++)
    {
        rtos_queue_send(tx_queue, &buf[i], 5);
    }

    /* Enable TX Irq */
    USART2->CR1 |= USART_CR1_TXEIE;

    return 0;
}

int32_t usart_read_buf(uint8_t* buf, const int32_t n_bytes)
{
    if((buf == NULL) || (n_bytes < 1))
    {
        /* Invalid arguments */
        return -EINVAL;
    }

    if(rtos_sem_take(rx_sem_bin, 10) != true)
    {
        /* Report timeout */
        return -EBUSY;
    }

    uint8_t bytes_read = 0;
    for(uint8_t i = 0; i < n_bytes; i++)
    {
        if(rtos_queue_receive(rx_queue, &buf[i], 5) != pdTRUE)
        {
            break;
        }

        bytes_read++;
    }

    return bytes_read;
}

void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    gpio_mode_config(GPIOA, USART_TX_PIN, GPIO_MODE_AF);
    gpio_mode_config(GPIOA, USART_RX_PIN, GPIO_MODE_AF);

    gpio_af_config(GPIOA, USART_TX_PIN, GPIO_AF_USART2);
    gpio_af_config(GPIOA, USART_RX_PIN, GPIO_AF_USART2);
}

void usart2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2->BRR = APB1_CLOCK_FREQ / USART_BAUD_RATE;           /* Calculate USART BaudRate */
    USART2->CR1 = USART_CR1_RXNEIE;                            /* Enable RX Irq */
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; /* Enable Receiver, Transmiter, and USART */

    NVIC_SetPriority(USART2_IRQn, USART_PRIORITY);
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)
{
    int32_t yield = 0;
    uint8_t tx_data, rx_data;

    if(USART2->SR & USART_SR_TXE)
    {
        if(rtos_queue_receive_isr(tx_queue, &tx_data, &yield) == pdTRUE)
        {
            USART2->DR = tx_data;
        }
        else
        {
            rtos_sem_give_isr(tx_sem_bin, yield);

            USART2->CR1 &= ~(USART_CR1_TXEIE);
        }
    }

    if(USART2->SR & USART_SR_RXNE)
    {
        rx_data = USART2->DR;

        if(rtos_queue_send_isr(rx_queue, &rx_data, &yield) == pdTRUE)
        {
            rtos_sem_give_isr(rx_sem_bin, yield);
        }
    }

    portYIELD_FROM_ISR(yield);
}
