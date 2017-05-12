#include "FreeRTOS.h"
#include "task.h"
#include "common.h"

void vLedsFloat(void* dummy)
{while(1){
  Led_Hi1();
  vTaskDelay(120/portTICK_RATE_MS);
  Led_Lo1();
  vTaskDelay(120/portTICK_RATE_MS);
 }
}

void vLedsFlash(void* dummy)
{while(1){
  Led_Hi2();
  vTaskDelay(301/portTICK_RATE_MS);
  Led_Lo2();
  vTaskDelay(301/portTICK_RATE_MS);
 }
}

/* Writes each 500 ms */
void vPrintUart(void* dummy)
{portTickType last_wakeup_time;
 last_wakeup_time = xTaskGetTickCount();
 while(1){uart_puts("Hello World\r\n");
	  vTaskDelayUntil(&last_wakeup_time, 500/portTICK_RATE_MS);
	}
}

int main(void){
 volatile int i;
 Usart1_Init(); // inits clock as well
 Led_Init();
 Led_Hi1();

 if (!(pdPASS == xTaskCreate( vLedsFloat, (signed char*) "LedFloat",64,NULL,1,NULL ))) goto hell;
 if (!(pdPASS == xTaskCreate( vLedsFlash, (signed char*) "LedFlash",64,NULL,2,NULL ))) goto hell;
 if (!(pdPASS == xTaskCreate( vPrintUart, (signed char*) "Uart",    64,NULL,3,NULL ))) goto hell;

 vTaskStartScheduler();
hell:              // should never be reached
	while(1);
    return 0;
}

