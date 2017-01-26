#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>

//#define avec_newlib

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif

void clock_setup(void);
void usart_setup(void);
void gpio_setup(void);

int main(void)
{ int i, c = 0;
  clock_setup();
  gpio_setup();
  usart_setup();
  while (1) {
    gpio_toggle(GPIOC, GPIO9);
    gpio_toggle(GPIOC, GPIO8);
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
#ifndef avec_newlib
    usart_send_blocking(USART1, c + '0'); // USART1: send byte
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
#else
    printf("%d\r\n", (int)c); 
#endif
    for (i = 0; i < 800000; i++) __asm__("NOP");
  }
  return 0;
}


