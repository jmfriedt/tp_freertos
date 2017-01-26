/* Include STM32 Firmware Lib Headers */
#include "common.h"
#include "stm32/usart.h"
#include "stm32/gpio.h"
#include "stm32/rcc.h"

/* Inits USART1 (serial line) */
void Usart1_Init(void)
{USART_InitTypeDef       usart_i;
 USART_ClockInitTypeDef  usart_c;
 GPIO_InitTypeDef 		gpio_i;

 RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,   ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_USART1,DISABLE);
	/* Enable needed clocks */
 RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,  ENABLE);

	/* TX - GPIO*/
 gpio_i.GPIO_Pin   = GPIO_Pin_9;
 gpio_i.GPIO_Speed = GPIO_Speed_50MHz;
 gpio_i.GPIO_Mode  = GPIO_Mode_AF_PP;
 GPIO_Init( GPIOA, &gpio_i);

	/* RX - GPIO*/
 gpio_i.GPIO_Pin   = GPIO_Pin_10;
 gpio_i.GPIO_Speed = GPIO_Speed_50MHz;
 gpio_i.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
 GPIO_Init( GPIOA, &gpio_i);

	/* Configure UART 115200-8N1 */
 usart_i.USART_BaudRate              = 115200;
 usart_i.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
 usart_i.USART_Mode		     = USART_Mode_Rx | USART_Mode_Tx;
 usart_i.USART_Parity	     	     = USART_Parity_No;
 usart_i.USART_StopBits	     	     = USART_StopBits_1;
 usart_i.USART_WordLength            = USART_WordLength_8b;

 usart_c.USART_Clock			= USART_Clock_Enable;
 usart_c.USART_CPHA			= USART_CPHA_1Edge;
 usart_c.USART_CPOL			= USART_CPOL_Low;
 usart_c.USART_LastBit			= USART_LastBit_Disable;

	/* Write configuration to registers */
 USART_ClockInit(USART1, &usart_c);
 USART_Init(USART1, &usart_i);
 USART_Cmd(USART1,ENABLE);
}

void Led_Hi(void) {GPIO_SetBits   (GPIOC, GPIO_Pin_9);}
void Led_Lo(void) {GPIO_ResetBits (GPIOC, GPIO_Pin_9);}

/* Configure the LED GPIOs*/
void Led_Init(void)
{GPIO_InitTypeDef GPIO_InitStructure;
    
	/* init Clocks */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Configure PC1-PC2 as output push-pull (LED) */
 GPIO_WriteBit(GPIOC,0x00000000,Bit_SET);
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|GPIO_Pin_2 |GPIO_Pin_8|GPIO_Pin_9;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* Writes one character over the serial line*/
void uart_putc(char c)
{while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};
 USART_SendData(USART1, c);
}

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c)
{while(*c!=0) uart_putc(*(c++));
}
