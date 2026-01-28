#include "common.h"
#include "../common/avec_newlib.h"

void disp8(char c)
{char tmp;
 tmp=(c>>4);  if (tmp<10) uart_putc(tmp + '0'); else uart_putc(tmp + 'A'-10);
 tmp=(c&0xf); if (tmp<10) uart_putc(tmp + '0'); else uart_putc(tmp + 'A'-10);
}

void disp16(short s) {disp8(s>>8); disp8(s&0xff); }

void disp32(short s) {disp16(s>>16); disp16(s&0xffff); }

#ifdef avec_newlib
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
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
    disp32(c); // USART: send byte
    uart_puts("\r\n\0");
#else
    printf("%08x\r\n", (int)c); 
#endif
    for (i = 0; i < 800000; i++) __asm__("NOP");
  }
  return 0;
}
