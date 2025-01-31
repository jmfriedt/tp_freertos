#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include "common.h"

#define usart1        // comment for STM32F4Discovery
//#define avec_newlib

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif

void clock_setup(void)
{
 rcc_clock_setup_in_hse_8mhz_out_72mhz();  // STM32F103
  rcc_periph_clock_enable(RCC_GPIOC); // Enable GPIOC clock
  rcc_periph_clock_enable(RCC_GPIOD); // Enable GPIOD clock for F4 (LEDs)
  rcc_periph_clock_enable(RCC_GPIOA); // Enable GPIOA clock
#ifdef usart1
  rcc_periph_clock_enable(RCC_USART1);
#else
  rcc_periph_clock_enable(RCC_USART2);
#endif
}


void Usart1_Init(void)
{ // Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
  clock_setup();
#ifdef usart1
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
#else
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
#endif

#ifdef usart1
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_enable(USART1);  // PA9 & PA10 for USART1
#else
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_enable(USART2);
#endif
}

// define newlib stub
#ifdef avec_newlib
int _write(int file, char *ptr, int len)
{ int i; 
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
     for (i = 0; i < len; i++) {
         if (ptr[i] == '\n') 
#ifdef usart1
              usart_send_blocking(USART1, '\r');
#else
              usart_send_blocking(USART2, '\r');
#endif
#ifdef usart1
         usart_send_blocking(USART1, ptr[i]);
#else
         usart_send_blocking(USART2, ptr[i]);
#endif
       }
      return i;
  }
  errno = EIO;
  return -1;
}
#endif

void uart_putc(char c) {
#ifdef usart1
usart_send_blocking(USART1, c); // USART1: send byte
#else
usart_send_blocking(USART2, c); // USART2: send byte
#endif
}

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c) {while(*c!=0) uart_putc(*(c++));}
