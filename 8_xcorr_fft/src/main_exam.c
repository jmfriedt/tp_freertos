#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "semphr.h"
#include "queue.h"
#include <stdint.h> 
#include <stdlib.h> // malloc
#include <stdio.h>  // sprintf

#include "arm_math.h"
#include "arm_const_structs.h"

#include "data.h"

#define N 32
// #define N 256

xQueueHandle qh1=0,qh2=0;

void hex(int i,char *c)
{int j;
 c[0]='0';c[1]='x';
 for (j=0;j<8;j++)
    {c[7-j+2]=(i>>(j*4))&0x0f;
     if (c[7-j+2]<10) c[7-j+2]+='0'; else c[7-j+2]+=('A'-10);
    }
 c[10]=0;
}

void cpl(int r,int i,char *c)
{hex(r,c);
 c[10]=' ';
 hex(i,&c[11]);
}

void do_display(int32_t *i, int n)
{int k;
 char c[25];
 uart_puts("disp=\0");hex(i[2*N],c); uart_puts(c);uart_puts("\n\0");
 for (k=0;k<n;k+=2)
   {cpl(i[k],i[k+1],c);
    uart_puts(c);
    uart_puts("\n\0");
   }
}

void do_mul(void* param)
{int32_t *in1,*in2,*in3,*meas,*c1,*c2,k;
 int64_t tmp1,tmp2;
 // char c[22];
 if(!xQueueReceive(qh1, &in1, portMAX_DELAY)) uart_puts("echec rx1\n\0");
// uart_puts("m1: \0");
// hex(in1[2*N],c); uart_puts(c);uart_puts("\n\0");

 if(!xQueueReceive(qh1, &in2, portMAX_DELAY)) uart_puts("echec rx2\n\0");
// hex(in2[2*N],c); uart_puts(c);uart_puts("\n\0");

 if(!xQueueReceive(qh1, &in3, portMAX_DELAY)) uart_puts("echec rx3\n\0");
// hex(in3[2*N],c); uart_puts(c);uart_puts("\n\0");

 switch (in1[2*N])
  {case 1:c1=in1;break;
   case 2:c2=in1;break;
   case 3:meas=in1;break;
   default: uart_puts("error\n\0");
  }
 switch (in2[2*N])
  {case 1:c1=in2;break;
   case 2:c2=in2;break;
   case 3:meas=in2;break;
   default: uart_puts("error\n\0");
  }
 switch (in3[2*N])
  {case 1:c1=in3;break;
   case 2:c2=in3;break;
   case 3:meas=in3;break;
   default: uart_puts("error\n\0");
  }
 
 for (k=0;k<2*N;k+=2) 
   {tmp1=(int64_t)c1[k]*(int64_t)meas[k]+(int64_t)c1[k+1]*(int64_t)meas[k+1];  // x.*conj(c1)
    tmp2=-(int64_t)c1[k+1]*(int64_t)meas[k]+(int64_t)c1[k]*(int64_t)meas[k+1]; // x.*conj(c1)
    c1[k]=(tmp1>>31);
    c1[k+1]=(tmp2>>31);

    tmp1=(int64_t)c2[k]*(int64_t)meas[k]+(int64_t)c2[k+1]*(int64_t)meas[k+1];  // x.*conj(c2)
    tmp2=-(int64_t)c2[k+1]*(int64_t)meas[k]+(int64_t)c2[k]*(int64_t)meas[k+1]; // x.*conj(c2)
    c2[k]=(tmp1>>31);
    c2[k+1]=(tmp2>>31);
   }
 do_display(c1, 2*N);
 do_display(c2, 2*N); // must be BEFORE queue that triggers iFFT !

 if(!xQueueSend(qh2, &c1, 100)) uart_puts("echec c1\n\0");
 if(!xQueueSend(qh2, &c2, 100)) uart_puts("echec c2\n\0");
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}
 
void do_ifft(void* p)
{int32_t *in,k,max=0,maxindex;
 char c[13];
// uart_puts("g0:\0");
 if(!xQueueReceive(qh2, &in, portMAX_DELAY));
#if N==32
 arm_cfft_q31(&arm_cfft_sR_q31_len32, (q31_t*)in, 1, 1); // FFT
#else
 arm_cfft_q31(&arm_cfft_sR_q31_len256, (q31_t*)in, 1, 1); // FFT
#endif
// uart_puts("g2: \0");
// hex(in[2*N],c); uart_puts(c);uart_puts(":\0");
 for (k=0;k<2*N;k+=2) 
   {if (in[k]>max) {max=in[k];maxindex=k;}
    // uart_puts("0x\0");hex(mod,c);
//    dec(mod,c);uart_puts(c);uart_puts("\n\0");
   }
 uart_puts("max=\0");hex(max,c); uart_puts(c);
 uart_puts(" @ \0");hex(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(in, 2*N);
 
 max=0;
 if(!xQueueReceive(qh2, &in, portMAX_DELAY));
#if N==32
 arm_cfft_q31(&arm_cfft_sR_q31_len32, (q31_t*)in, 1, 1); // FFT
#else
 arm_cfft_q31(&arm_cfft_sR_q31_len256, (q31_t*)in, 1, 1); // FFT
#endif
// uart_puts("g3: \0");
// hex(in[2*N],c); uart_puts(c);uart_puts(":\0");
 for (k=0;k<2*N;k+=2) 
   {if (in[k]>max) {max=in[k];maxindex=k;}
    // uart_puts("0x\0");hex(mod,c);
//    dec(mod,c);uart_puts(c);uart_puts("\n\0");
   }
 uart_puts("max=\0");hex(max,c); uart_puts(c);
 uart_puts(" @ \0");hex(maxindex,c); uart_puts(c);uart_puts("\n\0");
 do_display(in, 2*N);
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}

void rm_moy(int32_t* in)
{int32_t k,somme;
 // char c[20];
 somme=0;
 for (k=0;k<2*N;k+=2) somme+=in[k];
 somme/=N;
// uart_puts("somme\n\0"); hex(somme,c); uart_puts(c); uart_puts("\n\0");
 for (k=0;k<2*N;k+=2) in[k]-=somme;
 somme=0;
 for (k=1;k<2*N;k+=2) somme+=in[k];
 somme/=N;
 for (k=1;k<2*N;k+=2) in[k]-=somme;
}

void do_fft(void* param)
{char c[20];
 q31_t *in=param;
 uart_puts("f0\0");
 rm_moy(in);
 uart_puts("f1\0");
#if N==32
 arm_cfft_q31(&arm_cfft_sR_q31_len32, in, 0, 1); // FFT
#else
 arm_cfft_q31(&arm_cfft_sR_q31_len256, in, 0, 1); // FFT
#endif
 uart_puts("f2\0");
 if (in[2*N]>0)
    if(xQueueSend(qh1, &in, portMAX_DELAY)!= pdPASS) uart_puts("echec tx\n\0");
 uart_puts("fft");hex(in[2*N],c); uart_puts(c);uart_puts("\n\0");
 do_display(in, 2*N);
 while(1) {vTaskDelay(301/portTICK_RATE_MS);}
}   

int main()
{// static uint32_t c0[2*N],c1[2*N],c2[2*N],meas[2*N]; // N*4*2 bytes FFT crash
 static int32_t *c0,*c1,*c2,*meas; //  FFT functional
 int k;
 Usart1_Init(); // inits clock as well

 c0=(int32_t*)malloc((2*N+1)*sizeof(int32_t));
 c1=(int32_t*)malloc((2*N+1)*sizeof(int32_t));
 c2=(int32_t*)malloc((2*N+1)*sizeof(int32_t));
 meas=(int32_t*)malloc((2*N+1)*sizeof(int32_t));

 qh1=xQueueCreate(1, sizeof(int32_t*)); if (qh1==0) uart_puts("echec queue1\n\0");
 qh2=xQueueCreate(1, sizeof(int32_t*)); if (qh2==0) uart_puts("echec queue2\n\0");

 for (k=0;k<N;k++) {c1[2*k]=(pattern1[k]<<28); c1[2*k+1]=0;
                    c2[2*k]=(pattern2[k]<<28); c2[2*k+1]=0;
                    meas[2*k]=(measurement[k]<<19); meas[2*k+1]=0;
#if N==32
                    c0[2*k]=(sinus1[k]); c0[2*k+1]=0;  // sine wave to check FFT validity
#endif
                   }
c0[2*N]=0;
c1[2*N]=1;
c2[2*N]=2;
meas[2*N]=3;

#define S 4096

// if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft_test", STACK_BYTES(20*N), c0,  1,NULL ))) goto hell;
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft1", STACK_BYTES(S), c1,  1,NULL ))) {uart_puts("1\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fft2", STACK_BYTES(S), c2,  3,NULL ))) {uart_puts("2\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_fft, (const char*) "fftm", STACK_BYTES(S), meas,2,NULL ))) {uart_puts("3\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_mul, (const char*) "mul",  STACK_BYTES(S), NULL,1,NULL ))) {uart_puts("4\0");goto hell;}
 if (!(pdPASS == xTaskCreate( do_ifft,(const char*) "ifft", STACK_BYTES(S), NULL,1,NULL ))) {uart_puts("5\0");goto hell;}
 vTaskStartScheduler();
 hell:              // should never be reached
   uart_puts("Hell\n\0");
   while(1);
 return 0;
}
