#include "common.h"

// Function Declarations
int main(void)
{ int i, c = 0,k=0;
//  clock_setup();
  Led_Init();
  Usart1_Init(); //  usart_setup();
  while (1) {
    if (k==1) Led_Hi1(); else Led_Lo1();
    k=1-k;
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
    uart_putc(c + '0'); // USART1: send byte
    uart_putc('\r');
    uart_putc('\n');
    for (i = 0; i < 80000; i++) __asm__("NOP");
  }
  return 0;
}
