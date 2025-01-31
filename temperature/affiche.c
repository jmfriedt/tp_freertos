#include "common.h"

void affchar(char c)
{char b;
 b=((c&0xf0)>>4); if (b<10) uart_putc(b+'0'); else uart_putc(b+'A'-10); 
 b=(c&0x0f);      if (b<10) uart_putc(b+'0'); else uart_putc(b+'A'-10); 
}

void affshort(short s)
{affchar((s&0xff00)>>8);
 affchar(s&0xff);
}
