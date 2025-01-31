#include "common.h"

//#define avec_newlib
#ifdef avec_newlib
#include <stdio.h>
int _write(int file, char *ptr, int len);
#endif

int main(void)
{ int i, c = 0;
  Usart1_Init();
  Led_Init();
  while (1) {
    if (c&0x01) {Led_Hi1();Led_Hi2();} else {Led_Lo1();Led_Lo2();}
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
#ifndef avec_newlib
    uart_putc(c + '0'); // USART: send byte
    uart_puts("\r\n\0");
#else
    printf("%d\r\n", (int)c); 
#endif
    for (i = 0; i < 800000; i++) __asm__("NOP");
  }
  return 0;
}
