#include <libopencm3/stm32/gpio.h>

#include "gpio.h"


void init_Gpio(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RST_DDS | Led_Verte | Led_Jaune | Led_Rouge);
	
	//Définition spéciale pour le signal à 70MHz du DDS
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, TIM1_CH1);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, TIM1_CH1);
	gpio_set_af(GPIOA, GPIO_AF1, TIM1_CH1);
	
	//Définition des broches du SPI
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NSS | SLEEP);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SCK | MISO | MOSI);
	gpio_set_af(GPIOA, GPIO_AF5, SCK | MISO | MOSI);
	
	//Définition des broches pour la mesure
	gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PHASE | MAGNITUDE);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ENA_DET);
	
	//Définition des broches pour l'USART
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, RSRX | RSTX);
	gpio_set_af(GPIOA, GPIO_AF7, RSRX | RSTX);
	
}

void delay(uint64_t delay)
{
	uint64_t i;
	for(i=0; i<delay; i++)
	{
		__asm("nop");
	} 
}
