// 170308: ajout des conditions d'echec
// 180227: ajout du mutex pour faire des vraies phrases sans etre interrompu
// * on ne prend la 2eme baguette que si la 1ere a ete obtenue
// * on re-essaie tant qu'on n'a pas mange'
// * on n'a le droit de manger que si on a 2 baguettes (correction)
// TODO : etudier l'impact des diverses latences
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "common.h"

#define NB_PHILO 5
//#define avec

xSemaphoreHandle xMutex[NB_PHILO];
int mange[NB_PHILO];
#ifdef avec
xSemaphoreHandle xMutexEcrit;
#endif

void func(void* p)
{ int numero= *(int*) p;
  mange[numero]=0;
  while (mange[numero]!=1)
    {
#ifdef avec
     xSemaphoreTake(xMutexEcrit,portMAX_DELAY);
#endif
     uart_putc('0'+numero);
     uart_puts(" s'apprete a manger\n");
#ifdef avec
     xSemaphoreGive(xMutexEcrit);
#endif
     vTaskDelay(500 / portTICK_RATE_MS);
     if (xSemaphoreTake(xMutex[numero],500/portTICK_RATE_MS)!=pdFALSE)  // prend 1re  baguette
       {
#ifdef avec
        xSemaphoreTake(xMutexEcrit,portMAX_DELAY);
#endif
        uart_putc('0'+numero); //vTaskDelay(500 / portTICK_RATE_MS);   // prend 2eme baguette si 1ere ok
        uart_puts(" a pris 1 baguette\n");
#ifdef avec
        xSemaphoreGive(xMutexEcrit);
#endif
        if (xSemaphoreTake(xMutex[(numero+1)%NB_PHILO],500/portTICK_RATE_MS)==pdFALSE)
           {
#ifdef avec
            xSemaphoreTake(xMutexEcrit,portMAX_DELAY);
#endif
            uart_putc('0'+numero);
            uart_puts(" repose sa baguette\n");
#ifdef avec
            xSemaphoreGive(xMutexEcrit);
#endif
            // uart_puts("ng\r\n\0");
            xSemaphoreGive( xMutex[numero] );                          // sinon repose la 1ere baguette
           }
        else
           {
#ifdef avec
            xSemaphoreTake(xMutexEcrit,portMAX_DELAY); 
#endif
            uart_putc('0'+numero);
            uart_puts(" a pris 2 baguettes\n");
#ifdef avec
            xSemaphoreGive(xMutexEcrit);
#endif
	    vTaskDelay(500 / portTICK_RATE_MS);  // 2 baguettes = on mange
            xSemaphoreGive( xMutex[numero] );                          // on repose tout
            xSemaphoreGive( xMutex[(numero+1)%NB_PHILO] );
#ifdef avec
            xSemaphoreTake(xMutexEcrit,portMAX_DELAY); 
#endif
            uart_putc('0'+numero);
            uart_puts(" a mange\n");
#ifdef avec
            xSemaphoreGive(xMutexEcrit);
#endif
            mange[numero]=1;                                           // fini
           }
       } // on a rate' la 1ere baguette
    }
  while (1) { vTaskDelay(100 / portTICK_RATE_MS); }; // on n'a jamais le droit de finir toutes les taches
}

int main()
{ 
//www.freertos.org/FreeRTOS_Support_Forum_Archive/February_2007/freertos_Problems_with_passing_parameters_to_task_1666309.html
  static int p[NB_PHILO];
  static char *taskNames[NB_PHILO] = {"P0","P1","P2","P3","P4"};

  int i;
  Usart1_Init();
  Led_Init();
  for (i=0;i<NB_PHILO;i++) {xMutex[i] = xSemaphoreCreateMutex();p[i]=i;}
#ifdef avec
  xMutexEcrit=xSemaphoreCreateMutex();
#endif
  for (i=0;i<NB_PHILO;i++)
      {xTaskCreate(func, (const signed char const*)taskNames[i], STACK_BYTES(256), (void*)&p[i],1,0);}
  vTaskStartScheduler();
  while(1);
  return 0;
}
