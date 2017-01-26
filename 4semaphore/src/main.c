#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "common.h"

int globale=0;
xSemaphoreHandle event_signal;
 
void task1(void* p)
{
  while (1) {
//    if (xSemaphoreTake(event_signal,500/portTICK_RATE_MS)==pdFALSE)
//       uart_puts("not available\r\n\0");
//    else
//       uart_puts("sem take\r\n\0");

    while (globale==0) ; // vTaskDelay( 1/portTICK_RATE_MS );
    globale=0;
    uart_puts("sem take\r\n\0");
// ici on demontre que le semaphore est bien fait car il rend la main en cas de blocage,
// contrairement au cas de la variable globale qui, en l'absence de Delay, va bloquer sur
// la tache prioritaire qui ne rend pas la main
  }
}

void task2(void* p)
{
  while (1) {
//    xSemaphoreGive(event_signal); // debloque tache 1
    globale=1;
    uart_puts("sem give\r\n\0");
    vTaskDelay( 700/portTICK_RATE_MS );  // remplacer 400 par 700 !
  }
}

int main()
{
 Usart1_Init();
 uart_puts("depart\r\n\0");
//    vSemaphoreCreateBinary( event_signal );   // Create the semaphore
//    xSemaphoreTake(event_signal, 0);          // Take semaphore after creating it.
 xTaskCreate(task1, (signed char*)"t1", (256), 0, 2, 0);
 xTaskCreate(task2, (signed char*)"t2", (256), 0, 1, 0);
 vTaskStartScheduler();
 
hell:
    while(1) {};
    return 0;
}
