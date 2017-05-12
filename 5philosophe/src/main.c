// 170308: ajout des conditions d'echec
// * on ne prend la 2eme baguette que si la 1ere a ete obtenue
// * on re-essaie tant qu'on n'a pas mange'
// * on n'a le droit de manger que si on a 2 baguettes (correction)
// TODO : etudier l'impact des diverses latences
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "common.h"

#define NB_PHILO 5

xSemaphoreHandle xMutex[NB_PHILO];
int mange[NB_PHILO];

void func(void* p)
{ int numero= *(int*) p;
  mange[numero]=0;
  while (mange[numero]!=1)
    {uart_putc('a'+numero);vTaskDelay(500 / portTICK_RATE_MS);
     if (xSemaphoreTake(xMutex[numero],500/portTICK_RATE_MS)!=pdFALSE)  // prend 1re  baguette
       {uart_putc('v'+numero); //vTaskDelay(500 / portTICK_RATE_MS);   // prend 2eme baguette si 1ere ok
        if (xSemaphoreTake(xMutex[(numero+1)%NB_PHILO],500/portTICK_RATE_MS)==pdFALSE)
           {uart_putc('m'+numero);
            // uart_puts("ng\r\n\0");
            xSemaphoreGive( xMutex[numero] );                          // sinon repose la 1ere baguette
           }
        else
           {                                                         
            uart_putc('A'+numero);vTaskDelay(500 / portTICK_RATE_MS);  // 2 baguettes = on mange
            xSemaphoreGive( xMutex[numero] );                          // on repose tout
            xSemaphoreGive( xMutex[(numero+1)%NB_PHILO] );
            uart_putc('0'+numero);
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
  static char * taskNames[5] = {"P0","P1","P2","P3","P4"};

  int i;
  Usart1_Init();
  Led_Init();
  for (i=0;i<NB_PHILO;i++) {xMutex[i] = xSemaphoreCreateMutex();p[i]=i;}
  for (i=0;i<NB_PHILO;i++)
      {xTaskCreate(func, (const signed char const*)taskNames[i], STACK_BYTES(256), (void*)&p[i],1,0);}
  vTaskStartScheduler();
  while(1);
  return 0;
}
