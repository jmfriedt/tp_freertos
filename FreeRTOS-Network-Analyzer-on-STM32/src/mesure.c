#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

#include "mesure.h"
#include "gpio.h"

void init_Mesure(void)
{
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_clk_prescale(ADC_CCR_ADCPRE_BY4);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_112CYC);
	adc_power_on(ADC1);
	gpio_set(GPIOC, ENA_DET);
}

uint16_t read_Mag(void)
{
	uint8_t channel[16];
	channel[0]=9;
	adc_set_regular_sequence(ADC1, 1, channel);
	adc_start_conversion_regular(ADC1);
	while(!adc_eoc(ADC1));
	
	return (adc_read_regular(ADC1));
}


uint16_t read_Phase(void)
{
	uint8_t channel[16];
	channel[0]=8;
	adc_set_regular_sequence(ADC1, 1, channel);
	adc_start_conversion_regular(ADC1);
	while(!adc_eoc(ADC1));
	
	return (adc_read_regular(ADC1));
}

