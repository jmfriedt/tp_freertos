#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/f4/memorymap.h>
#include "common.h"


//#define avec_newlib

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif

int main(void)
{ int i, c = 0;
  Led_Init();
  Usart1_Init();
  while (1) {
    if (c&0x01) Led_Hi(); else Led_Lo();
    gpio_toggle(GPIOC, GPIO9);
    gpio_toggle(GPIOC, GPIO8);
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
#ifndef avec_newlib
    uart_putc(c + '0'); // USART1: send byte
    uart_puts("\r\n\0");
#else
    printf("%d\r\n", (int)c); 
#endif
    for (i = 0; i < 800000; i++) __asm__("NOP");
  }
  return 0;
}
