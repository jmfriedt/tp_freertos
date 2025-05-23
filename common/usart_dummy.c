#include "avec_newlib.h"
#include "common.h"

#ifdef avec_newlib
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int _write(int file, char *ptr, int len);
#endif


void clock_setup(void)
{
}


void Usart1_Init(void)
{
}

void Led_Init(void)
{
}

void Led_Hi1(void) {}
void Led_Lo1(void) {}
void Led_Hi2(void) {}
void Led_Lo2(void) {}

// define newlib stub
#ifdef avec_newlib
int _write(int file, char *ptr, int len)
{ int i; 
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
     for (i = 0; i < len; i++) {
//         if (ptr[i] == '\n') 
//              usart_send_blocking(USART1, '\r');
//         usart_send_blocking(USART1, ptr[i]);
       }
      return i;
  }
  errno = EIO;
  return -1;
}
#endif

void uart_putc(char c) {
//usart_send_blocking(USART2, c); // USART2: send byte
}

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c) {while(*c!=0) uart_putc(*(c++));}
