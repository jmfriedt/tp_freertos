#include <libopencm3/lm4f/systemcontrol.h>
#include <libopencm3/lm4f/rcc.h>
#include <libopencm3/lm4f/gpio.h>
#include <libopencm3/lm4f/uart.h>
#include <libopencm3/lm4f/timer.h>

#include <stdint.h>
#include "avec_newlib.h"
#include "common.h"

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif

void clock_setup(void)
{
//	rcc_sysclk_config(OSCSRC_MOSC, XTAL_16M, 5) ; // PLL_DIV_80MHZ); // from GPIO
}


void Usart1_Init(void)
{ 
	periph_clock_enable(RCC_GPIOA);       // Enable GPIOA in run mode. 
	gpio_set_af(GPIOA, 1, GPIO0 | GPIO1); // Mux PA0 and PA1 to UART0 (alternate function 1) 
	periph_clock_enable(RCC_UART0);       // Enable the UART clock 
	__asm__("nop");                       // delay before access UART config registers 
	uart_disable(UART0);                  // Disable the UART while setting
	uart_clock_from_piosc(UART0);         // UART clock source as precision internal oscillator 
	uart_set_baudrate(UART0, 115200);     // Set communication parameters 
	uart_set_databits(UART0, 8);
	uart_set_parity(UART0, UART_PARITY_NONE);
	uart_set_stopbits(UART0, 1);
	uart_enable(UART0);                  
}

void Led_Init(void)
{
	gpio_enable_ahb_aperture();
	periph_clock_enable(RCC_GPIOF);
	gpio_mode_setup(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2 | GPIO1);
	gpio_set_output_config(GPIOF, GPIO_OTYPE_PP, GPIO_DRIVE_2MA, GPIO2 | GPIO1);
}

void Led_Hi1(void) {gpio_set  (GPIOF, GPIO2);}
void Led_Lo1(void) {gpio_clear(GPIOF, GPIO2);}
void Led_Hi2(void) {gpio_set  (GPIOF, GPIO1);}
void Led_Lo2(void) {gpio_clear(GPIOF, GPIO1);}

// define newlib stub
#ifdef avec_newlib
int _write(int file, char *ptr, int len)
{ int i; 
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
     for (i = 0; i < len; i++) {
         if (ptr[i] == '\n') 
              uart_send_blocking(UART0, '\r');
         uart_send_blocking(UART0, ptr[i]);
       }
      return i;
  }
  errno = EIO;
  return -1;
}
#endif

void uart_putc(char c) 
{uart_send_blocking(UART0, c); 
}

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c) {while(*c!=0) uart_putc(*(c++));}
