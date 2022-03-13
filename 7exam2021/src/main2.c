#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "semphr.h"
#include <stdio.h>

#define N 5
#define avec_sema

#ifdef avec_sema
xSemaphoreHandle xMutex[N-1];
#endif

struct cpl {int re;int im;};
struct argument{struct cpl v[3];int indice;};

struct cpl cmul(struct cpl c1,struct cpl c2)
{struct cpl cres;
 cres.re=c1.re*c2.re-c2.im*c2.im;
 cres.im=c1.re*c2.im+c1.im*c2.re;
 return(cres);
}

struct cpl cadd(struct cpl c1,struct cpl c2)
{struct cpl cres;
 cres.re=c1.re+c2.re;
 cres.im=c1.im+c2.im;
 return(cres);
}

void mandel(void* dummy)
{struct argument *c=(struct argument*)dummy;
 struct cpl c1=c->v[0];
 struct cpl c2=c->v[1];
 struct cpl cres=c->v[2];
 int indice=c->indice;
 char resultat[32];
#ifdef avec_sema
 if (indice!=0) xSemaphoreTake(xMutex[indice-1],500/portTICK_RATE_MS);
#endif
 resultat[0]=indice+'0';
 resultat[1]=0;
 uart_puts(resultat);
 uart_puts("Hello: ");

 resultat[0]=c1.re+'0'; resultat[1]=c1.im+'0'; resultat[2]='+'; resultat[3]=0;
 uart_puts(resultat);
 resultat[0]=c2.re+'0'; resultat[1]=c2.im+'0'; resultat[2]='='; resultat[3]=0;
 uart_puts(resultat);

// while(1) 
 {cres=cadd(c1,c2);
  resultat[0]=cres.re+'0'; resultat[1]=cres.im+'0'; resultat[2]='\n'; resultat[3]=0;
  uart_puts(resultat);
#ifdef avec_sema
  xSemaphoreGive(xMutex[indice]);
#endif
  while (1) {vTaskDelay(101/portTICK_RATE_MS);}
 }
}

int main(void){
 int k;
 static struct argument c[N]; // ={{.re=1.,.im=2.},{.re=3.,.im=4.}};
 Usart1_Init(); // inits clock as well
#ifdef avec_sema
 for (k=0;k<N-1;k++) {xMutex[k] = xSemaphoreCreateMutex(); xSemaphoreTake(xMutex[k],500/portTICK_RATE_MS);}
#endif
 for (k=0;k<N;k++)
    {
     c[k].v[0].re=1;
     c[k].v[0].im=2;
     c[k].v[1].re=k;
     c[k].v[1].im=k;
     c[k].indice=k;
     if (!(pdPASS == xTaskCreate( mandel, (const char*) "mandel",64,&c[k],1,NULL ))) goto hell;
    }

 vTaskStartScheduler();
hell:              // should never be reached
        uart_puts("Hell\n");
	while(1);
    return 0;
}
