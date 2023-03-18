#include "avr/io.h"
#include "avr/interrupt.h"

#define F_CPU 16000000UL
#define USART_BAUDRATE (9600)

void Usart1_Init(void)
{ unsigned short baud;
  USBCON=0; // desactive l'interruption USB

  UCSR1A = 0;                         // importantly U2X1 = 0
  UCSR1B = 0;                         
  UCSR1B = (1 << RXEN1)|(1 << TXEN1); // enable receiver and transmitter
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // 8N1
  // UCSR1D = 0;                         // no rtc/cts (probleme de version de libavr)
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;
  sei();
}

void Led_Hi1(void) {PORTB |= (1<<PORTB5);}
void Led_Lo1(void) {PORTB &= ~(1<<PORTB5);}
void Led_Hi2(void) {PORTE |= (1<<PORTE6);}
void Led_Lo2(void) {PORTE &= ~(1<<PORTE6);}

void Led_Init(void)
{ DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB &= ~(1<<PORTB5);
  PORTE &= ~(1<<PORTE6);
}

void uart_putc(char c)
{while (!(UCSR1A&(1<<UDRE1))) ;
 UDR1 = c;
}

void uart_puts(char *c)
{while(*c!=0) uart_putc(*(c++));
}
