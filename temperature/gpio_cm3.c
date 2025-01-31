#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "common.h"

#define usart1        // comment for STM32F4Discovery
//#define avec_newlib

void Led_Init(void)
{
 gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO1|GPIO2); // lecteur SENSeOR: GPIO1&2=LED
}

// qemu: ../../qemu/qemu_stm32/hw/arm/stm32_p103.c 
//    led_irq = qemu_allocate_irqs(led_irq_handler, NULL, 1);
//    qdev_connect_gpio_out(gpio_c, 1, led_irq[0]);
void Led_Hi1(void) {gpio_set  (GPIOC, GPIO2);}
void Led_Lo1(void) {gpio_clear(GPIOC, GPIO2);}
void Led_Hi2(void) {gpio_set  (GPIOC, GPIO1);}
void Led_Lo2(void) {gpio_clear(GPIOC, GPIO1);}
