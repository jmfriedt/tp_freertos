#include <libopencm3/stm32/rcc.h>

#include <libopencm3/stm32/f4/memorymap.h>

#include <libopencm3/stm32/flash.h> // definitions du timer


#include "stm32_initialisation.h"

// APB2 max=84 MHz but when the APB prescaler is NOT 1, the interface clock is fed
// twice the frequency => Sysclk = 140 MHz, APB2=2 but Timers are driven at twice that is 140.
const struct rcc_clock_scale rcc_hse_20mhz_3v3 = {
                .pllm = 20,                           // 20/20=1 MHz
                .plln = 280,                          // 1*280/2=140 MHz
                .pllp = 2,                            //       ^
                .pllq = 6,
                .hpre = RCC_CFGR_HPRE_DIV_NONE,
                .ppre1 = RCC_CFGR_PPRE_DIV_4,
                .ppre2 = RCC_CFGR_PPRE_DIV_2,
                .flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE |
                                FLASH_ACR_LATENCY_4WS, // 4 WS d'apres configuration par ST
                .ahb_frequency  = 140000000,  
                .apb1_frequency = 35000000,    
                .apb2_frequency = 70000000,
        };

/**
  * @file    system_stm32f4xx.c
  * @author  MCD Application Team
  *=============================================================================
  *        Supported STM32F40xx/41xx/427x/437x devices
  *        System Clock source                    | PLL (HSE)
  *        SYSCLK(Hz)                             | 140000000
  *        HCLK(Hz)                               | 140000000
  *        AHB Prescaler                          | 1
  *        APB1 Prescaler                         | 4
  *        APB2 Prescaler                         | 2
  *        HSE Frequency(Hz)                      | 20000000
  *        PLL_M                                  | 20
  *        PLL_N                                  | 280
  *        PLL_P                                  | 2
  *        PLL_Q                                  | 6
  *        Flash Latency(WS)                      | 4
  *        Require 48MHz for USB OTG FS,          | Disabled
  *        SDIO and RNG clock                     |
*/

void core_clock_setup(void)
{rcc_clock_setup_hse_3v3(&rcc_hse_20mhz_3v3); // custom version
}

void clock_setup (void) {
	//rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_120MHZ]);
	rcc_periph_clock_enable (RCC_GPIOA);
	rcc_periph_clock_enable (RCC_GPIOB);
	rcc_periph_clock_enable (RCC_GPIOC);
	rcc_periph_clock_enable (RCC_TIM1);
	rcc_periph_clock_enable (RCC_SPI1);
	rcc_periph_clock_enable (RCC_ADC1);
	rcc_periph_clock_enable (RCC_USART1);
} 
