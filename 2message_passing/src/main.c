#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "common.h"
#include <stm32/gpio.h>

void vLedsFloat(void* dummy);
void vLedsFlash(void* dummy);
void vPrintUart(void* dummy);

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

xQueueHandle qh = 0;
 
void task_tx(void* p)
{int myInt = 0;
  while(1)
    {myInt++;
     if(!xQueueSend(qh, &myInt, 100)) 
        {uart_puts("Failed to send item to queue within 500ms");
        }
     vTaskDelay(1000);
    }
}
 
void task_rx(void* p)
{char c[10];
 int myInt = 0;
 while(1)
    {if(!xQueueReceive(qh, &myInt, 1000)) 
       {uart_puts("Failed to receive item within 1000 ms");
       }
     else {c[0]='0'+myInt;c[1]=0;
           uart_puts("Received: ");uart_puts(c);uart_puts("\r\n");
          }
    }
}

int main()
{Led_Init();
 Usart1_Init();

  qh = xQueueCreate(1, sizeof(int));

//  activer ces fonctions fait atteindre le timeout de transfert de donnees dans la queue
//    if (!(pdPASS == xTaskCreate( vLedsFloat, ( signed char * ) "LedFloat", 128, NULL, 2, NULL ))) goto hell;
//    if (!(pdPASS == xTaskCreate( vLedsFlash, ( signed char * ) "LedFlash", 128, NULL, 2, NULL ))) goto hell;
//    if (!(pdPASS == xTaskCreate( vPrintUart, ( signed char * ) "Uart",     128, NULL, 2, NULL ))) goto hell;

   xTaskCreate(task_tx, (signed char*)"t1", (128), 0, 2, 0);
   xTaskCreate(task_rx, (signed char*)"t2", (128), 0, 2, 0);
   vTaskStartScheduler();
hell: while(1) {};
    return 0;
}

