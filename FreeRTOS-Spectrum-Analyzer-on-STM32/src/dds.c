#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "dds.h"
#include "gpio.h"

void init_SPI(void)
{
	spi_reset(SPI1);
	spi_disable(SPI1);
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_256,
					SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1, 
					SPI_CR1_DFF_16BIT, SPI_CR1_MSBFIRST );
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);
	spi_enable(SPI1);
}

void init_DDS(void)
{
	init_SPI();
	gpio_clear(GPIOA, SLEEP);
	//Reset du DDS
	gpio_set(GPIOA, RST_DDS | NSS);
	delay(0xFFFFF);
	gpio_clear(GPIOA, RST_DDS);
	delay(0XFFFFF);
	
	gpio_clear(GPIOA, NSS);
	spi_send(SPI1, 0x2228);//DB13-1/DB9-1/DB5-1/DB3-1	
	delay(0x500);
	
	gpio_set(GPIOA, NSS);
}

void set_Freq(uint32_t outFrequency, uint8_t inFrequency)
{
	volatile uint64_t value;
	value=(outFrequency*268)/inFrequency;
	set_FreqWord((uint32_t)value&0xFFFFFFFF);
	
}

void set_FreqWord(uint32_t frequency)
{
	gpio_clear(GPIOA, NSS);
	spi_send(SPI1, (frequency&0x3FFF)|0x4000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
	
	delay(0x500);
	
	gpio_clear(GPIOA, NSS);
	spi_send(SPI1, ((frequency>>14)&0x3FFF)|0x4000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
	
	spi_send(SPI1, (frequency&0x3FFF)|0x8000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
	
	delay(0x500); 
	
	gpio_clear(GPIOA, NSS);
	spi_send(SPI1, ((frequency>>14)&0x3FFF)|0x8000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
}

void set_Phase(uint16_t value)
{
	gpio_clear(GPIOA, NSS);
	spi_send(SPI1, (value&0xFFF)|0xC000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
	
	delay(0x500);
	
	
	spi_send(SPI1, (value&0xFFF)|0xE000);
	delay(0x500);
	gpio_set(GPIOA, NSS);
	
	delay(0x500);
	

}
