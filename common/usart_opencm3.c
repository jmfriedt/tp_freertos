#include <libopencm3/cm3/common.h> // BEGIN_DECL,          added 150116
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include "common.h"
void clock_setup(void);

//#define avec_newlib

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif

void clock_setup(void)
{ rcc_clock_setup_in_hse_8mhz_out_24mhz();
  rcc_periph_clock_enable(RCC_GPIOC); // Enable GPIOC clock
  rcc_periph_clock_enable(RCC_GPIOA); // Enable GPIOA clock
  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_ADC1);  // exemple ADC
}

void Usart1_Init(void)
{ // Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
  clock_setup();
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

  usart_enable(USART1);
}

void Led_Init(void)
{gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO8|GPIO9);}

void Led_Hi(void) {gpio_set  (GPIOC, GPIO9);}
void Led_Lo(void) {gpio_clear(GPIOC, GPIO9);}

// define newlib stub
#ifdef avec_newlib
int _write(int file, char *ptr, int len)
{ int i; 
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
     for (i = 0; i < len; i++) {
         if (ptr[i] == '\n') {usart_send_blocking(USART1, '\r');}
         usart_send_blocking(USART1, ptr[i]);
       }
      return i;
  }
  errno = EIO;
  return -1;
}
#endif

void uart_putc(char c) {usart_send_blocking(USART1, c);} // USART1: send byte

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c) {while(*c!=0) uart_putc(*(c++));}
