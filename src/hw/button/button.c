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

void button_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    gpio_mode_config(GPIOE, DISPLAY_SCREEN_SWAP_PIN, GPIO_MODE_INPUT);
    gpio_pupd_config(GPIOE, DISPLAY_SCREEN_SWAP_PIN, GPIO_PUPD_PU);
}

uint32_t button_1_get(void)
{
    return (GPIOE->IDR & (1 << DISPLAY_SCREEN_SWAP_PIN)) == 0;
}