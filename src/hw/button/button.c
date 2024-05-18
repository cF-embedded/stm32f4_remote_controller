/**
 * @file button.c
 * @author cF-embedded (cf@embedded.pl)
 * @brief   Hardware driver for buttons with EXTI.
 *
 * @copyright Copyright (c) 2024
 *
 */
/*
 * file:	button.c
 * author:	GAndaLF
 * brief:	Hardware driver for buttons.
 */

#include "button.h"
#include "gpio_f4.h"
#include "platform_specific.h"

/** Switch 1 pin number - PE06 */
#define DISPLAY_SCREEN_SWAP_PIN 6

/** PE pin in EXTI SYSCFG */
#define EXTICR_PE 4
/** Pin 6 of EXTI SYSCFG */
#define EXTICR2_PIN_6 8

/** Flag for display screen swap */
static volatile uint32_t display_screen_swap;

void button_init(void)
{
    display_screen_swap = 0;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    gpio_mode_config(GPIOE, DISPLAY_SCREEN_SWAP_PIN, GPIO_MODE_INPUT);
    gpio_pupd_config(GPIOE, DISPLAY_SCREEN_SWAP_PIN, GPIO_PUPD_PD);

    SYSCFG->EXTICR[1] = (EXTICR_PE << EXTICR2_PIN_6); /** SYSCFG PE6 Externall Interrupt */
    EXTI->IMR = EXTI_IMR_MR6;                         /** Unmasked IRQ on EXTI 6 */
    EXTI->RTSR = EXTI_RTSR_TR6;                       /** Rising Trigger */

    NVIC_SetPriority(EXTI9_5_IRQn, USART_PRIORITY);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

uint32_t button_1_get(void)
{
    uint32_t result_val = display_screen_swap;
    /* Clear flag */
    display_screen_swap = 0;

    return result_val;
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR & EXTI_PR_PR6)
    {
        /* Clear EXIT PR6 flag */
        EXTI->PR = EXTI_PR_PR6;
        /* Set display screen swap flag */
        display_screen_swap = 1;
    }
}