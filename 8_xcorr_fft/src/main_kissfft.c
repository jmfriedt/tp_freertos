#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "semphr.h"
#include "queue.h"
#include <stdint.h> 
#include <stdlib.h> // malloc
#include <stdio.h>  // sprintf

#include "kiss_fft.h"
#include "../kissfft/_kiss_fft_guts.h"
#include "data.h"

#define N 128
//#define N 32

void __malloc_lock(struct _reent *REENT)   {vPortEnterCritical();}
void __malloc_unlock(struct _reent *REENT) {vPortExitCritical();}

xQueueHandle qh1=0,qh2=0;
xSemaphoreHandle xMutex;

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pTaskName)
{uart_puts("\r\nStack: ");
 uart_puts(pTaskName);
 while(1) vTaskDelay(301/portTICK_RATE_MS);
}

void dec(int32_t i,char *c)
{int32_t v,d=1000000000;
 int k=1;
 if (i<0) {c[0]='-';i=-i;} else c[0]='+';
 while (d>=1)
   {v=(i/d);
    c[k]=(char)v+'0';
    i=i-v*d;
    d/=10;
    k++;
  }
 c[k]=0;
}

void hex(int i,char *c)
{int j;
 for (j=0;j<8;j++)
    {c[7-j]=(i>>(j*4))&0x0f;
     if (c[7-j]<10) c[7-j]+='0'; else c[7-j]+=('A'-10);
    }
 c[8]=0;
}

void cpl(int r,int i,char *c)
{dec(r,c);
 c[11]=' ';
 dec(i,&c[12]);
}

void do_display_r(kiss_fft_cpx *i, int n)
{int k;
 char c[25];
 for (k=0;k<n;k++)
   {dec(i[k].r,c); // {hex(i[k],&c[2]);
    uart_puts(c);
    uart_puts("\n\0");
   }
}

void do_display(kiss_fft_cpx *i, int n)
{int k;
 char c[80];
 uart_puts("disp=\0");hex(i[N].r,c); uart_puts(c);uart_puts(" n=\0");
 hex(n,c); uart_puts(c);uart_puts(" \n\0");
 for (k=0;k<n;k++)
   {cpl(i[k].r,i[k].i,c); // {hex(i[k],&c[2]);
    uart_puts(c);
    uart_puts("\n\0");
   }
}

void do_mul(void* param)
{kiss_fft_cpx *in1,*in2,*in3,*meas,*c1,*c2;
 int k;
 int64_t tmp1,tmp2;
 if(!xQueueReceive(qh1, &in1, portMAX_DELAY)) uart_puts("echec rx1\n\0");
 if(!xQueueReceive(qh1, &in2, portMAX_DELAY)) uart_puts("echec rx2\n\0");
 if(!xQueueReceive(qh1, &in3, portMAX_DELAY)) uart_puts("echec rx3\n\0");
 switch ((int)in1[N].r)
  {case 1:c1=in1;break;
   case 2:c2=in1;break;
   case 3:meas=in1;break;
   default: uart_puts("error\n\0");while(1);
  }
 switch ((int)in2[N].r)
  {case 1:c1=in2;break;
   case 2:c2=in2;break;
   case 3:meas=in2;break;
   default: uart_puts("error\n\0");while(1);
  }
 switch ((int)in3[N].r)
  {case 1:c1=in3;break;
   case 2:c2=in3;break;
   case 3:meas=in3;break;
   default: uart_puts("error\n\0");while(1);
  }
// do_display(c1->in,  N);
// do_display(c2->in,  N);
// do_display(meas->in,N);
 for (k=0;k<N;k++) 
   {tmp1=(int64_t)c1[k].r*(int64_t)meas[k].r+(int64_t)c1[k].i*(int64_t)meas[k].i;  // x.*conj(c1)
    tmp2=-(int64_t)c1[k].i*(int64_t)meas[k].r+(int64_t)c1[k].r*(int64_t)meas[k].i; // x.*conj(c1)
    c1[k].r=(tmp1>>30);
    c1[k].i=(tmp2>>30);

    tmp1=(int64_t)c2[k].r*(int64_t)meas[k].r+(int64_t)c2[k].i*(int64_t)meas[k].i;  // x.*conj(c2)
    tmp2=-(int64_t)c2[k].i*(int64_t)meas[k].r+(int64_t)c2[k].r*(int64_t)meas[k].i; // x.*conj(c2)
    c2[k].r=(tmp1>>32);
    c2[k].i=(tmp2>>32);
   }
// do_display(c1->out, N);
// do_display(c2->out, N); // must be BEFORE queue that triggers iFFT !

 if(!xQueueSend(qh2, &c1, 100)) uart_puts("echec c1\n\0");
 if(!xQueueSend(qh2, &c2, 100)) uart_puts("echec c2\n\0");
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}
 
void do_ifft(void* p)
{
 kiss_fft_cpx *in;
 kiss_fft_cpx *out;
 kiss_fft_cfg cfg;
 int32_t k,max=0,maxindex;
 char c[13];
 out =(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
 cfg=kiss_fft_alloc( N ,1  ,NULL,NULL );
 if(!xQueueReceive(qh2, &in, portMAX_DELAY));
 kiss_fft(cfg , in , out );
 for (k=0;k<N;k++) 
   {if (out[k].r>max) {max=out[k].r;maxindex=k;}
   }
 uart_puts("max=\0");dec(max,c); uart_puts(c);
 uart_puts(" @ \0");dec(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(out, N);
 
 max=0;
 if(!xQueueReceive(qh2, &in, portMAX_DELAY));
 kiss_fft(cfg , in , out );
 for (k=0;k<N;k++) 
   {if (out[k].r>max) {max=out[k].r;maxindex=k;}
   } uart_puts("max=\0");dec(max,c); uart_puts(c);
 uart_puts(" @ \0");dec(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(out, N);
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}

void rm_moy(kiss_fft_cpx* in)
{int32_t k,sommer,sommei;
 // char c[20];
 sommer=0;sommei=0;
 for (k=0;k<N;k++) {sommer+=in[k].r;sommei+=in[k].i;}
 sommer/=N;sommei/=N;
 for (k=0;k<N;k++) {in[k].r-=sommer;in[k].i-=sommei;}
}

void do_fft(void* param)
{char c[20];
 kiss_fft_cpx *in=(kiss_fft_cpx*)param;
 kiss_fft_cpx *out;
 kiss_fft_cfg cfg;

 xSemaphoreTake( xMutex, portMAX_DELAY );
 uart_puts("\nfft\0");hex(in[N].r,c); uart_puts(c);uart_puts(": \0");
 out =(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
 cfg=kiss_fft_alloc( N ,0  ,NULL,NULL );
 hex(cfg->nfft,c); uart_puts(c);uart_puts(": \0");
 rm_moy(in);
 uart_puts(" f1-\0");
 kiss_fft(cfg , in , out );
 kiss_fft_free(cfg);
 uart_puts("f2 \0");
 out[N].r=in[N].r;
 if (in[N].r>0)
    if(xQueueSend(qh1, &out, portMAX_DELAY)!= pdPASS) uart_puts("echec tx\n\0");
 uart_puts("end: ");hex(in[N].r,c); uart_puts(c);uart_puts("\n\0");
// do_display(out, N);
 xSemaphoreGive( xMutex );
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}   

//#define configUSE_TRACE_FACILITY        1
//#define configUSE_STATS_FORMATTING_FUNCTIONS    1
void do_ps(void* dummy)
{char c[50*7];
 portTickType last_wakeup_time;
 last_wakeup_time = xTaskGetTickCount();
 while(1){
          vTaskDelayUntil(&last_wakeup_time, 4000/portTICK_RATE_MS);
          vTaskList(c);
          uart_puts(c);
        }
}

int main()
{int k;
 unsigned int len;
 static kiss_fft_cpx *c0,*c1,*c2,*meas,*ci; // N*4*2 bytes
 Usart1_Init(); // inits clock as well
#if N==32
 c0=(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
#endif
 c1=(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
 c2=(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
 ci=(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));
 meas=(kiss_fft_cpx*)pvPortMalloc((N+1)*sizeof(kiss_fft_cpx));

 qh1=xQueueCreate(1, sizeof(kiss_fft_cpx*)); if (qh1==0) uart_puts("echec queue1\n\0");
 qh2=xQueueCreate(1, sizeof(kiss_fft_cpx*)); if (qh2==0) uart_puts("echec queue2\n\0");
 xMutex = xSemaphoreCreateMutex();

 for (k=0;k<N;k++) {c1[k].r=(pattern1[k]<<28); c1[k].i=0;
                    c2[k].r=(pattern2[k]<<28); c2[k].i=0;
                    meas[k].r=(measurement[k]<<10); meas[k].i=0;
#if N==32
                    c0[k].r=(sinus2[k]); c0[k].i=0;
#endif
                   }
#if N==32
c0[N].r=0;
#endif
c1[N].r=1;
c2[N].r=2;
meas[N].r=3;

#define S 1024

uart_putc('0'+sizeof(kiss_fft_cpx));

#if N==32
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft_test", STACK_BYTES(S), c0,  1,NULL ))) goto hell;
#endif
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft1", STACK_BYTES(S), c1,  1,NULL ))) {uart_puts("1\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft2", STACK_BYTES(S), c2,  1,NULL ))) {uart_puts("2\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fftm", STACK_BYTES(S), meas,1,NULL ))) {uart_puts("3\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_mul, (const char*) "mul",  STACK_BYTES(S), NULL, 1,NULL ))) {uart_puts("4\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_ifft,(const char*) "ifft", STACK_BYTES(S), NULL ,1,NULL ))) {uart_puts("5\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_ps,  (const char*) "ps",   STACK_BYTES(1024), NULL,1,NULL ))) {uart_puts("6\0");goto hell;}
 vTaskStartScheduler();
 hell:              // should never be reached
   uart_puts("Hell\n\0");
   while(1);
 return 0;
}
