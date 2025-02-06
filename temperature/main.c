#include "common.h"
#include <libopencm3/stm32/usart.h>
#include "affiche.h"

//#define avec_newlib
#ifdef avec_newlib
#include <stdio.h>
int _write(int file, char *ptr, int len);
#endif

int main(void)
{volatile int i, c = 0;
#ifdef avec_newlib
 volatile float f=2.0;
#endif
  short res;
  Usart1_Init();
  Led_Init();
  adc_setup();
  
  while (1) {
    if (c&0x01) {Led_Hi1();Led_Hi2();} else {Led_Lo1();Led_Lo2();}
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
    res=read_adc_naiive(1);
#ifndef avec_newlib
    uart_putc('0'); // USART: send byte
    uart_putc('x'); // USART: send byte

    affshort(res);
    uart_putc(' ');
    uart_putc(c + '0'); // USART: send byte
    uart_putc('\n'); uart_putc('\r');
    Led_Hi2(); 
    for (i = 0; i < 800000; i++) __asm__("NOP");
    Led_Lo2();
#else
    printf("%d %f\r\n", (int)res,sqrt(f)); 
#endif
  }
  return 0;
}


