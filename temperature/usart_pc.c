#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "common.h"

static int temps;

void Usart1_Init(void) {printf("USART init\n");}
void Led_Init(void)    {printf("LED init\n");}
void Led_Hi1(void)     {printf("LED1 hi\n");}
void Led_Lo1(void)     {printf("LED1 lo\n");}
void Led_Hi2(void)     {printf("LED2 hi\n");}
void Led_Lo2(void)     {printf("LED2 lo\n");}
void uart_putc(char c) {printf("%c", c);}
void uart_puts(char *c){while(*c!=0) uart_putc(*(c++));}
void adc_setup(void)   {printf("ADC init\n");temps=0;}
unsigned short read_adc_naiive(unsigned char chan)
{unsigned short val;
 temps++;
 val=(short)(2048.*(1.+sin(2.*M_PI*temps*500./10000.)));
 printf("0x%x ", val);
 val+=((rand()-RAND_MAX/2)/(RAND_MAX/2048))/10; // 32768/1024=32
 return(val);
}

