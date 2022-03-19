#include <stdint.h> 
#include <stdlib.h> // malloc
#include <stdio.h>  // sprintf
#include "../../common/common.h"
#include "../src/data.h"

#include "kiss_fft.h"
#define N 256

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

void do_display(kiss_fft_cpx *i, int n)
{int k;
 char c[25];
 uart_puts("disp=\0");hex(i[N].r,c); uart_puts(c);uart_puts("\n\0");
 for (k=0;k<n;k++)
   {cpl(i[k].r,i[k].i,c); // {hex(i[k],&c[2]);
    uart_puts(c);
    uart_puts("\n\0");
   }
}

void do_fft(kiss_fft_cpx *in,kiss_fft_cpx *out,int direction)
{char c[20];
 kiss_fft_cfg cfg;
 uart_puts("fft\0");
/* solution 1
 if (direction==0) kfc_fft(N,in,out);
   else            kfc_ifft(N,in,out);
 kfc_cleanup();
*/

/* solution 2 */
 if ((cfg = kiss_fft_alloc(N, direction, NULL, NULL)) != NULL)
   {kiss_fft( cfg , in , out );
    kiss_fft_free(cfg);
   }
 else uart_puts("FFT out of memory\0");
 uart_putc(':');hex(in[N].r,c); uart_puts(c);uart_puts("\n\0");
}   

void mul(kiss_fft_cpx *in1,kiss_fft_cpx *in2) // returns the result in in1
{int k;
 long long tmpr,tmpi;
 for (k=0;k<N;k++) 
    {tmpr=(long long)in1[k].r*(long long)in2[k].r+(long long)in1[k].i*(long long)in2[k].i;
     tmpi=(long long)in1[k].r*(long long)in2[k].i-(long long)in1[k].i*(long long)in2[k].r;
     in1[k].r=tmpr>>31;
     in1[k].i=tmpi>>31;
    }
}

int main()
{kiss_fft_cpx *in,*out1,*out2,*outm;
 int k;
 char c[9];
 Usart1_Init(); // inits clock as well
 in =(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 out1 =(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 out2 =(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));
 outm =(kiss_fft_cpx*)malloc((N+1)*sizeof(kiss_fft_cpx));

 for (k=0;k<N;k++) {in[k].r=(pattern1[k]<<30);in[k].i=0;}
 for (k=0;k<3;k++) 
    {hex(k,c);uart_puts(c);uart_putc(':');
     do_fft(in,out1,0); 
    }
 
 for (k=0;k<N;k++) {in[k].r=(pattern2[k]<<30);in[k].i=0;}
 for (k=0;k<3;k++) 
    {hex(k,c);uart_puts(c);uart_putc(':');
     do_fft(in,out2,0); 
    }
 
 for (k=0;k<N;k++) {in[k].r=(measurement[k]<<10);in[k].i=0;}
 for (k=0;k<3;k++) 
    {hex(k,c);uart_puts(c);uart_putc(':');
     do_fft(in,outm,0); 
    }

 mul(out1,outm);
 for (k=0;k<3;k++) 
    {hex(k,c);uart_puts(c);uart_putc(':');
     do_fft(out1,in,1); 
    }
 do_display(in,N);
 
 mul(out2,outm);
 for (k=0;k<3;k++) 
    {hex(k,c);uart_puts(c);uart_putc(':');
     do_fft(out2,in,1); 
    }
 do_display(in,N);
}
