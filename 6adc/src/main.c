#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "common.h"

#include <stm32/gpio.h>
#include <stm32/adc.h>

#define avec_mutex

unsigned short readADC1(unsigned char );
void initADC1();

xSemaphoreHandle xMutex1, xMutex2;

void vLedsFloat(void* dummy);
void vLedsFlash(void* dummy);
void vPrintUart(void* dummy);

unsigned int tempe[256];

void writeHEXc(unsigned char ptr)
{
  unsigned char b;
  b=((ptr&0xf0)>>4);
  if (b<10) uart_putc(b+48); else uart_putc(b+55);
  b=((ptr&0x0f));
  if (b<10) uart_putc(b+48); else uart_putc(b+55);
}

void writeHEXi(uint16_t val)
{
  writeHEXc(val>>8);
  writeHEXc(val&0xff);
}

void vGetsTemp(void* dummy)
{
  volatile int i=0,k;
  while(1){
    uart_puts("GetTemp\r\n");
#ifdef avec_mutex
    xSemaphoreTake(xMutex1, portMAX_DELAY);
#endif

    for (i=255;i>0;i--){
//      tempe[255-i]=readADC1(ADC_Channel_1);
        for (k=0x8fff;k>0;k--){}
    }
#ifdef avec_mutex
    xSemaphoreGive(xMutex2);
#endif
  }
}

/* Writes each 250 ms */
void vPrintUart(void* dummy)
{
  int i=0;
  portTickType last_wakeup_time;
  last_wakeup_time = xTaskGetTickCount();

  while(1){
    uart_puts("UART\r\n");
#ifdef avec_mutex
    xSemaphoreTake(xMutex2, portMAX_DELAY);
#else
    vTaskDelayUntil(&last_wakeup_time, 5/portTICK_RATE_MS);
#endif
    for (i=255;i>0;i--){
   //   writeHEXi(tempe[255-i]);uart_putc(' ');
      vTaskDelayUntil(&last_wakeup_time, 1/portTICK_RATE_MS);
    }
#ifdef avec_mutex
    xSemaphoreGive(xMutex1);
#endif

  }
}


int main(void){
//  Led_Init();
  Usart1_Init();
/*
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1 ,ENABLE) ;

  // Configure USART1 RX (PA. 1 0 ) as input floating 

  GPIO_InitStructure . GPIO_Pin = GPIO_Pin_10 ;
  GPIO_InitStructure . GPIO_Mode = GPIO_Mode_IN_FLOATING ;
  GPIO_Init (GPIOA, &GPIO_InitStructure );
*/
  initADC1(); // Configure ADC1

#ifdef avec_mutex
  xMutex1 = xSemaphoreCreateMutex();
  xMutex2 = xSemaphoreCreateMutex();
#endif

  if (!(pdPASS == xTaskCreate( vGetsTemp, (signed char*) "LedFloat",128,NULL,10,NULL ))) goto hell;
  if (!(pdPASS == xTaskCreate( vPrintUart, (signed char*) "Uart",   128,NULL,10,NULL ))) goto hell;

  vTaskStartScheduler();

 hell:              // should never be reached
  while(1);
  return 0;
}

