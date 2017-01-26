#include <libopencm3/cm3/common.h> // BEGIN_DECL,          added 150116
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>

//#define avec_newlib

void clock_setup(void);
void usart_setup(void);
void gpio_setup(void);

void pwm_init_timer(volatile uint32_t *reg, uint32_t en, uint32_t timer_peripheral, uint32_t prescaler, uint32_t period)
{
      rcc_peripheral_enable_clock(reg, en);

      timer_reset(timer_peripheral);

      timer_set_mode(timer_peripheral,
                     TIM_CR1_CKD_CK_INT,
                     TIM_CR1_CMS_EDGE,
                     TIM_CR1_DIR_UP);

       timer_set_prescaler(timer_peripheral, prescaler);
       timer_set_repetition_counter(timer_peripheral, 0);
       timer_enable_preload(timer_peripheral);
       timer_continuous_mode(timer_peripheral);
       timer_set_period(timer_peripheral, period);
}

void pwm_init_output_channel(uint32_t timer_peripheral, enum tim_oc_id oc_id,
     volatile uint32_t *gpio_reg, uint32_t gpio_en, uint32_t gpio_port, uint16_t gpio_pin)
{
       rcc_peripheral_enable_clock(gpio_reg, gpio_en);

       gpio_set_mode(gpio_port, GPIO_MODE_OUTPUT_50_MHZ,
                     GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                     gpio_pin);

       timer_disable_oc_output(timer_peripheral, oc_id);
       timer_set_oc_mode(timer_peripheral, oc_id, TIM_OCM_PWM1);
       timer_set_oc_value(timer_peripheral, oc_id, 0);
       timer_enable_oc_output(timer_peripheral, oc_id);
}

void pwm_start_timer(uint32_t timer_peripheral)
{timer_enable_counter(timer_peripheral);}


int main(void)
{ int i, c = 0;
  unsigned short k=1500,l=0,d=100;
  clock_setup();
  gpio_setup();
  usart_setup();

  pwm_init_timer(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN, TIM2, 24, 20000);
  pwm_init_output_channel(TIM2, TIM_OC3, &RCC_APB2ENR, RCC_APB2ENR_IOPAEN, GPIOA, GPIO_TIM2_CH3);
  timer_enable_counter(TIM2);
  timer_set_oc_value(TIM2, TIM_OC3, 1600);
  pwm_start_timer(TIM2);
  
  while (1) {

  l++;
  if (l==5) {l=0 ;
    k+=d; if ((k<=800) || (k>=2300)) {d=-d;k+=2*d;}
    timer_set_oc_value(TIM2, TIM_OC3, k);
   }

    gpio_toggle(GPIOC, GPIO9);
    gpio_toggle(GPIOC, GPIO8);
    c = (c == 9) ? 0 : c + 1;	// cyclic increment c
#ifndef avec_newlib
    usart_send_blocking(USART1, c + '0'); // USART1: send byte
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
#else
    printf("%d\r\n", (int)c); 
#endif
    for (i = 0; i < 80000; i++) __asm__("NOP");
  }
  return 0;
}
