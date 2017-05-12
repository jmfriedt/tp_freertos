#include <libopencm3/stm32/usart.h>

#include "usart.h"

void init_Usart(void)
{
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable(USART1);
}

void putChar(uint8_t ch)
{
	usart_send_blocking(USART1, ch);
}

void putString(uint8_t* buf)
{
	volatile uint8_t i=0;
	
	while(buf[i]!='\0')
	{
		putChar(buf[i]);
		i++;
	}
}

void putHexa16(uint16_t var)
{
	volatile uint8_t i;
	for(i=0; i<4; i++)
	{
		uint8_t val=((var>>(4*(3-i)))&0x000F);
		if(val<10)
			putChar(val+'0');
		else
			putChar(val-10+'A');
	}
}

void putHexa32(uint32_t var)
{
	volatile uint8_t i;
	for(i=0; i<8; i++)
	{
		uint8_t val=((var>>(4*(7-i)))&0x0000000F);
		if(val<10)
			putChar(val+'0');
		else
			putChar(val-10+'A');
	}
}

void putDec16(uint16_t var)
{
	volatile uint8_t dixMille, mille, cent, dix;
	dixMille=var/10000;
	var-=dixMille*10000;
	mille=var/1000;
	var-=mille*1000;
	cent=var/100;
	var-=cent*100;
	dix=var/10;
	var-=dix*10;
	
	putChar(dixMille+'0');
	putChar(mille+'0');
	putChar(cent+'0');
	putChar(dix+'0');
	putChar(var+'0');
		
}

void putDec32(uint32_t var)
{
	volatile uint8_t milliard, centMillions, dixMillions, millions, centMille, dixMille, mille, cent, dix;
	milliard=var/1000000000;
	var-=milliard*1000000000;
	centMillions=var/100000000;
	var-=centMillions*100000000;
	dixMillions=var/10000000;
	var-=dixMillions*10000000;
	millions=var/1000000;
	var-=millions*1000000;
	centMille=var/100000;
	var-=centMille*100000;
	dixMille=var/10000;
	var-=dixMille*10000;
	mille=var/1000;
	var-=mille*1000;
	cent=var/100;
	var-=cent*100;
	dix=var/10;
	var-=dix*10;
	
	putChar(milliard+'0');
	putChar(centMillions+'0');
	putChar(dixMillions+'0');
	putChar(millions+'0');
	putChar(centMille+'0');
	putChar(dixMille+'0');
	putChar(mille+'0');
	putChar(cent+'0');
	putChar(dix+'0');
	putChar(var+'0');
		
}

uint8_t getCharBlocking(void)
{
	return usart_recv_blocking(USART1)&0XFF;
}

uint8_t getChar(void)
{
	return usart_recv(USART1)&0XFF;
}

void getString(uint8_t* buf, uint8_t size)
{
	uint8_t i=0, temp;
	while(i<size)
	{
		temp=getChar();
		if(temp=='\n')
			break;
		else
			buf[i]=temp;
	}
}
