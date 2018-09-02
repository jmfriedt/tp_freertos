#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include "timer.h"

void init_Timer(uint8_t frequencyMHz)
{
	// timer_reset(TIM1); // old libopencm3
        rcc_periph_reset_pulse(RST_TIM1);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM2);
	timer_enable_oc_output(TIM1, TIM_OC1);
	timer_enable_break_main_output(TIM1);
	timer_set_oc_value(TIM1, TIM_OC1, 1);
	timer_set_prescaler(TIM1, (70/frequencyMHz)-1);
	timer_set_period(TIM1, 1);
	timer_enable_counter(TIM1);
}

void setTimer(uint8_t frequencyMHz)
{
	timer_set_prescaler(TIM1, (70/frequencyMHz)-1);
}



