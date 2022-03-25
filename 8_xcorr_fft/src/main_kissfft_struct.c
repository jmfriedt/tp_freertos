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

#define N 64 // or 128
// #define N 32

xQueueHandle qh1=0,qh2=0;
xSemaphoreHandle xMutex;

struct mast {char *cfg;kiss_fft_cpx *in; kiss_fft_cpx *out;};

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pTaskName)
{
 while(1){uart_puts("\r\nStack: ");
          uart_puts(pTaskName);
          vTaskDelay(301/portTICK_RATE_MS);
         }
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
 xSemaphoreTake( xMutex, portMAX_DELAY );
 for (k=0;k<n;k++)
   {dec(i[k].r,c); // {hex(i[k],&c[2]);
    uart_puts(c);
    uart_puts("\n\0");
   }
 xSemaphoreGive( xMutex );
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
{struct mast *in1,*in2,*in3,*meas,*c1,*c2;
 int k;
 int64_t tmp1,tmp2;
 if(!xQueueReceive(qh1, &in1, portMAX_DELAY)) uart_puts("echec rx1\n\0");
 if(!xQueueReceive(qh1, &in2, portMAX_DELAY)) uart_puts("echec rx2\n\0");
 if(!xQueueReceive(qh1, &in3, portMAX_DELAY)) uart_puts("echec rx3\n\0");
 switch ((int)in1->in[N].r)
  {case 1:c1=in1;break;
   case 2:c2=in1;break;
   case 3:meas=in1;break;
   default: uart_puts("error\n\0");
  }
 switch ((int)in2->in[N].r)
  {case 1:c1=in2;break;
   case 2:c2=in2;break;
   case 3:meas=in2;break;
   default: uart_puts("error\n\0");
  }
 switch ((int)in3->in[N].r)
  {case 1:c1=in3;break;
   case 2:c2=in3;break;
   case 3:meas=in3;break;
   default: uart_puts("error\n\0");
  }
// do_display(c1->in,  N);
// do_display(c2->in,  N);
// do_display(meas->in,N);
 for (k=0;k<N;k++) 
   {tmp1=(int64_t)c1->out[k].r*(int64_t)meas->out[k].r+(int64_t)c1->out[k].i*(int64_t)meas->out[k].i;  // x.*conj(c1)
    tmp2=-(int64_t)c1->out[k].i*(int64_t)meas->out[k].r+(int64_t)c1->out[k].r*(int64_t)meas->out[k].i; // x.*conj(c1)
    c1->in[k].r=(tmp1>>16);
    c1->in[k].i=(tmp2>>16);

    tmp1=(int64_t)c2->out[k].r*(int64_t)meas->out[k].r+(int64_t)c2->out[k].i*(int64_t)meas->out[k].i;  // x.*conj(c2)
    tmp2=-(int64_t)c2->out[k].i*(int64_t)meas->out[k].r+(int64_t)c2->out[k].r*(int64_t)meas->out[k].i; // x.*conj(c2)
    c2->in[k].r=(tmp1>>16);
    c2->in[k].i=(tmp2>>16);
   }
// do_display(c1->out, N);
// do_display(c2->out, N); // must be BEFORE queue that triggers iFFT !

 if(!xQueueSend(qh2, &c1, 100)) uart_puts("echec c1\n\0");
 if(!xQueueSend(qh2, &c2, 100)) uart_puts("echec c2\n\0");
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}
 
void do_ifft(void* p)
{struct mast *st=(struct mast*)p;
 struct mast *q;
 kiss_fft_cpx *in=st->in;
 kiss_fft_cpx *out=st->out;
 kiss_fft_cfg cfg=(kiss_fft_cfg)st->cfg;
 int32_t k,max=0,maxindex;
 char c[13];
 if(!xQueueReceive(qh2, &q, portMAX_DELAY));
 for (k=0;k<N+1;k++) {st->in[k].r=q->in[k].r; st->in[k].i=q->in[k].i; }
 kiss_fft(cfg , in , out );
 for (k=0;k<N;k++) 
   {if (st->out[k].r>max) {max=st->out[k].r;maxindex=k;}
   }
 uart_puts("max=\0");dec(max,c); uart_puts(c);
 uart_puts(" @ \0");dec(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(st->out, N);
 
 max=0;
 if(!xQueueReceive(qh2, &q, portMAX_DELAY));
 for (k=0;k<N+1;k++) {st->in[k].r=q->in[k].r; st->in[k].i=q->in[k].i; }
 kiss_fft(cfg , in , out );
 for (k=0;k<N;k++) 
   {if (st->out[k].r>max) {max=st->out[k].r;maxindex=k;}
   } uart_puts("max=\0");dec(max,c); uart_puts(c);
 uart_puts(" @ \0");dec(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(st->out, N);
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
 struct mast *ma=(struct mast*)param;
 kiss_fft_cpx *in=ma->in;
 kiss_fft_cpx *out=ma->out;
 kiss_fft_cfg cfg=(kiss_fft_cfg)ma->cfg;

 xSemaphoreTake( xMutex, portMAX_DELAY );
 uart_puts("\nfft\0");hex(in[N].r,c); uart_puts(c);uart_puts(": \0");
 hex(cfg->nfft,c); uart_puts(c);uart_puts(": \0");
 rm_moy(in);
 uart_puts(" f1-\0");
 kiss_fft(cfg , in , out );
// kiss_fft_free(cfg);
 uart_puts("f2 \0");
 if (in[N].r>0)
    if(xQueueSend(qh1, &ma, portMAX_DELAY)!= pdPASS) uart_puts("echec tx\n\0");
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
          vTaskDelayUntil(&last_wakeup_time, 2000/portTICK_RATE_MS);
          vTaskList(c);
          uart_puts(c);
        }
}

#define leninit 0x510

int main()
{int k;
 unsigned int len;
 static struct mast c0,c1,c2,meas,ci; // N*4*2 bytes

 Usart1_Init(); // inits clock as well
 
#if N==32
 c0.cfg=(char*)malloc(leninit);
#endif
 c1.cfg=(char*)malloc(leninit);   if (c1.cfg==NULL) uart_puts("\nc1 NULL\n\0");
 c2.cfg=(char*)malloc(leninit);   if (c2.cfg==NULL) uart_puts("\nc2 NULL\n\0");
 meas.cfg=(char*)malloc(leninit); if (meas.cfg==NULL) uart_puts("\nmeas NULL\n\0");
 ci.cfg=(char*)malloc(leninit);
 if (ci.cfg==NULL) uart_puts("\nci NULL\n\0");
#if N==32
 c0.in=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
#endif
 c1.in=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 c2.in=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 ci.in=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 meas.in=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
#if N==32
 c0.out=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
#endif
 c1.out=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 c2.out=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 ci.out=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 meas.out=(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
#if N==32
 len=leninit; kiss_fft_alloc( N ,0  ,c0.cfg ,&len );
#endif
 len=leninit; kiss_fft_alloc( N ,0  ,c1.cfg ,&len );
 len=leninit; kiss_fft_alloc( N ,0  ,c2.cfg ,&len );
 len=leninit; kiss_fft_alloc( N ,0  ,meas.cfg ,&len );
 len=leninit; kiss_fft_alloc( N ,1  ,ci.cfg ,&len );

 qh1=xQueueCreate(1, sizeof(struct mast)); if (qh1==0) uart_puts("echec queue1\n\0");
 qh2=xQueueCreate(1, sizeof(struct mast)); if (qh2==0) uart_puts("echec queue2\n\0");
 xMutex = xSemaphoreCreateMutex();

 for (k=0;k<N;k++) {c1.in[k].r=(pattern1[k]<<10); c1.in[k].i=0;
                    c2.in[k].r=(pattern2[k]<<10); c2.in[k].i=0;
                    meas.in[k].r=(measurement[k]<<6); meas.in[k].i=0;
#if N==32
                    c0.in[k].r=(sinus2[k]); c0.in[k].i=0;
#endif
                   }
#if N==32
c0.in[N].r=0;
#endif
c1.in[N].r=1;
c2.in[N].r=2;
meas.in[N].r=3;

#define S 1024

uart_putc('0'+sizeof(kiss_fft_cpx));

#if N==32
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft_test", STACK_BYTES(S), &c0,  1,NULL ))) goto hell;
#endif
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft1", STACK_BYTES(S), &c1,  1,NULL ))) {uart_puts("1\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft2", STACK_BYTES(S), &c2,  1,NULL ))) {uart_puts("2\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fftm", STACK_BYTES(S), &meas,1,NULL ))) {uart_puts("3\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_mul, (const char*) "mul",  STACK_BYTES(S), NULL, 1,NULL ))) {uart_puts("4\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_ifft,(const char*) "ifft", STACK_BYTES(S), &ci  ,1,NULL ))) {uart_puts("5\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_ps,  (const char*) "ps",   STACK_BYTES(1024), NULL,1,NULL ))) {uart_puts("6\0");goto hell;}
 vTaskStartScheduler();
 hell:              // should never be reached
   uart_puts("Hell\n\0");
   while(1);
 return 0;
}
