#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include "common.h"
void clock_setup(void);

//#define usart1
//#define avec_newlib

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif


void clock_setup(void)
{
#ifdef STM32F1
#ifdef STM32F10X_LD_VL
 rcc_clock_setup_in_hse_8mhz_out_24mhz();  // STM32F100 discovery
#else
 rcc_clock_setup_in_hse_8mhz_out_72mhz();  // STM32F103
#endif
#else
 rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]); 
#endif
  rcc_periph_clock_enable(RCC_GPIOC); // Enable GPIOC clock
  rcc_periph_clock_enable(RCC_GPIOD); // Enable GPIOD clock for F4 (LEDs)
  rcc_periph_clock_enable(RCC_GPIOA); // Enable GPIOA clock
#ifdef usart1
  rcc_periph_clock_enable(RCC_USART1);
#else
  rcc_periph_clock_enable(RCC_USART2);
#endif
  rcc_periph_clock_enable(RCC_ADC1);  // exemple ADC
}


void Usart1_Init(void)
{ // Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
  clock_setup();
#ifdef STM32F1
#ifdef usart1
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
#else
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
#endif
#else
  gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);  //GPA9 : Tx send from STM32 to ext
  gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10); //GPD10: Rx recieve from ext to STM32
  gpio_set_af (GPIOA, GPIO_AF7, GPIO9);
  gpio_set_af (GPIOA, GPIO_AF7, GPIO10);
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

void Led_Init(void)
{
#ifdef STM32F1
 gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO8|GPIO9|GPIO1|GPIO2|GPIO12);
#else
 gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, GPIO12|GPIO13|GPIO14|GPIO15); 
#endif
}

#ifdef STM32F1
void Led_Hi1(void) {gpio_set  (GPIOC, GPIO9);gpio_set  (GPIOC, GPIO2);gpio_set  (GPIOC, GPIO12);}
void Led_Lo1(void) {gpio_clear(GPIOC, GPIO9);gpio_clear(GPIOC, GPIO2);gpio_clear(GPIOC, GPIO12);}
void Led_Hi2(void) {gpio_set  (GPIOC, GPIO8);gpio_set  (GPIOC, GPIO1);}
void Led_Lo2(void) {gpio_clear(GPIOC, GPIO8);gpio_clear(GPIOC, GPIO1);}
#else
void Led_Hi1(void) {gpio_set  (GPIOD, GPIO12);}
void Led_Lo1(void) {gpio_clear(GPIOD, GPIO12);}
void Led_Hi2(void) {gpio_set  (GPIOD, GPIO13);}
void Led_Lo2(void) {gpio_clear(GPIOD, GPIO13);}
#endif

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
