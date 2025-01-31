#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>

void adc_setup(void)
{	rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_ADC1);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO1);

        /* Make sure the ADC doesn't run during config. */

        adc_power_off(ADC1);
        adc_disable_scan_mode(ADC1);
        adc_set_single_conversion_mode(ADC1);
        adc_disable_external_trigger_regular(ADC1);
        adc_set_right_aligned(ADC1);
        adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

        adc_power_on(ADC1);

        /* Wait for ADC starting up. */
        int i;
        for (i = 0; i < 800000; i++) /* Wait a bit. */
                __asm__("nop");

        adc_reset_calibration(ADC1);
        adc_calibrate(ADC1);
}

unsigned short read_adc_naiive(unsigned char channel)
{int c;
  unsigned char channel_array[16];
  channel_array[0] = channel;
  adc_set_regular_sequence(ADC1, 1, channel_array);
// while (!adc_eoc(ADC1));
//  adc_set_sample_time(ADC1, channel, ADC_SMPR_SMP_1DOT5CYC);
//  adc_start_conversion_direct(ADC1);
 adc_start_conversion_regular(ADC1);
 while (! adc_eoc(ADC1));
 // do {c=*(long*)ADC1_BASE;} while ((c & ADC_SR_EOC) ==0);

 return adc_read_regular(ADC1);
}
