// gcc -Wall -O2 -o exec main_pthread_Linux.c -lpthread
#include <stdio.h>
#include "stdint.h"
#include <pthread.h>
#include <unistd.h>

#define uart_putc(x) printf("%c",x)

#ifdef fl   // define as Makefile argument -Dfl for floating point calculation
typedef float type;
#define SCALE 1.
#else
typedef int32_t type;
#define SCALE 1000
#endif

#define P 2
#define Q 2
#define IND 0  // segment qu'on affiche
#define ITER 1 // nombre de fois qu'on repete le calcul : 300 pour benchmark

struct cpl {type re;type im;};

struct cpl mulcomp(struct cpl in1,struct cpl in2)
{struct cpl tmp;
 tmp.re=in1.re*in2.re-in1.im*in2.im;
 tmp.im=in1.re*in2.im+in1.im*in2.re;
 tmp.re/=SCALE;
 tmp.im/=SCALE;
// tmp.re>>=10;
// tmp.im>>=10;
 return(tmp);
}

struct cpl addcomp(struct cpl in1,struct cpl in2)
{struct cpl tmp;
 tmp.re=in1.re+in2.re;
 tmp.im=in1.im+in2.im;
 return(tmp);
}

type modcomp(struct cpl in1)
{type tmp;
 tmp=in1.re*in1.re+in1.im*in1.im;
 return(tmp/SCALE);
// return(tmp>>10);
}

#undef avec_sema

#ifdef avec_sema
xSemaphoreHandle xMutex[P*Q-1];
#endif

struct argument{struct cpl v[2];int indice;}; // coins de debut et de fin

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

void* mandelbrot(void* param)
{volatile struct cpl tmp;
 volatile struct cpl current;
 volatile int n; 
 volatile int compte; // benchmark

 struct argument *c=(struct argument*)param;
 struct cpl c1=c->v[0];
 struct cpl c2=c->v[1];
 int indice=c->indice;
 type zr,zi,seuil;
 seuil=(type)(10.*SCALE);
// clock_setup();
// usart_setup();
//gpio_clear(GPIOD,GPIO15|GPIO12); // benchmark with GPIO toggle
#ifdef fl
//uart_putc('y');
#else
//uart_putc('x');
#endif
//uart_putc(indice+'0');
 for (compte=0;compte<ITER;compte++)  // benchmark
{
 for (zi=c1.im;zi<c2.im;zi+=(type)(0.02*SCALE))
   {for (zr=c1.re;zr<c2.re;zr+=(type)(.02*SCALE))
     {n=0;
      current.re=zr;
      current.im=zi;
      tmp.re=zr;
      tmp.im=zi;
      do {
          tmp=addcomp(current,mulcomp(tmp,tmp));
          n++; 
         }
      while ((modcomp(tmp)<seuil)&&(n<16));
      if (indice==IND)
         {if (n<16) uart_putc('0'+n); else uart_putc(' ');}  // benchmark
     }
     if (indice==IND) uart_putc('\n'); 
   }
}
#ifdef fl
if (indice==IND) uart_putc('Y');
#else
if (indice==IND) uart_putc('X');
#endif
uart_putc(indice+'0');
//while (1) ;
return(0);
}   

int main()
{int x,y,k;
 static struct argument c[P*Q]; // ={{.re=1.,.im=2.},{.re=3.,.im=4.}};
 pthread_t thread_id[P*Q];
// Usart1_Init(); // inits clock as well
#ifdef avec_sema
 for (k=0;k<P*Q-1;k++) {xMutex[k] = xSemaphoreCreateMutex(); xSemaphoreTake(xMutex[k],500/portTICK_RATE_MS);}
#endif
 k=0;
 for (x=0;x<P;x++)
   {for (y=0;y<Q;y++)
      {c[k].indice=k;
       c[k].v[0].re=(type)(-2.*SCALE)+(x*(type)(2.85*SCALE))/(type)P;     // 2.85=.85-(-2)
       c[k].v[1].re=(type)(-2.*SCALE)+((x+1)*(type)(2.85*SCALE))/(type)P;
       c[k].v[0].im=(type)(-1.2*SCALE)+(y*(type)(2.4*SCALE))/(type)Q;     // 2.4=1.2-(-1.2)
       c[k].v[1].im=(type)(-1.2*SCALE)+((y+1)*(type)(2.4*SCALE))/(type)Q;  
       k++;
      }
   }
 for (k=0;k<P*Q;k++)
     pthread_create( &thread_id[k], NULL, mandelbrot, &c[k] );
 for (k=0;k<P*Q;k++) {pthread_join( thread_id[k], NULL);}     // ATTENTE DE LA MORT DE CES THREADS
/*       if (!(pdPASS == xTaskCreate( mandelbrot, (const char*) "mandel",64,&c[k],1,NULL ))) goto hell;
 vTaskStartScheduler();
hell:              // should never be reached
        uart_puts("Hell\n");
	while(1);
*/
    return 0;
}

