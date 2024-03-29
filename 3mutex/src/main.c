#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "common.h"
#include "stdlib.h" // rand

//#define avec_mutex

volatile int global=0;
#ifdef avec_mutex
xSemaphoreHandle xMutex;
// SemaphoreHandle_t xMutex; // pour FreeRTOSv9
#endif

void task_rx(void* p)
{   char aff[10];
    char *t=(char*)p;
    int myInt = 0;
    volatile int local;
    for (myInt=0;myInt<8;myInt++)
    {
#ifdef avec_mutex
    xSemaphoreTake( xMutex, portMAX_DELAY );
#endif
         local=global;
         local++;
         uart_puts(t);  
// cette operation nous fait perdre beaucoup de temps ... il y a toutes les
// chances pour que la seconde tache se lance pendant cet intervalle de temps ... mais la seconde tache
// va cherche global qui n'a pas encore ete incremente' et le resultat sera errone' puisque les 16 taches
// auront fait une somme de 8
         global=local;
    
#ifdef avec_mutex
    xSemaphoreGive( xMutex );
#endif
    vTaskDelay( ( rand() & 0x5 ) );  // essayer de deplacer le delay sous cette '}' pour ne pas alterner
    }
    aff[0]=' ';aff[1]=global+'0';aff[2]=' ';aff[3]=0;uart_puts(aff);
    while (1) vTaskDelay( ( rand() & 0x5 ) ); // on n'a jamais le droit de finir toutes les taches
}

int main()
{
    Usart1_Init();
    Led_Init();
    srand( 567 );
#ifdef avec_mutex
  xMutex = xSemaphoreCreateMutex();
#endif
    xTaskCreate(task_rx, (signed char*)"t1", 128, "1111111111111111111111111111111111111111111\r\n\0", 1, 0);
    xTaskCreate(task_rx, (signed char*)"t2", 128, "2222222222222222222222222222222222222222222\r\n\0", 1, 0);
    vTaskStartScheduler();
 
hell:
    while(1);
   return 0;
}
