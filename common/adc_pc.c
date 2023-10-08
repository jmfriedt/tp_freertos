#include <stdio.h>
#include <math.h>

#define fs 48000.
#define f  440.

unsigned short readADC1(unsigned char channel)
{static float phase;                              // unknown initial state
 if ((phase>2*M_PI) || (phase<-2*M_PI)) phase=0.; // make sure phase starts between +/- 2pi.
 phase+=2*M_PI*f/fs; if (phase>2*M_PI) phase-=2*M_PI;
 return((short)(32767.*sin(phase)));
}

void initADC1(void)
{printf("ADC initialized\n");
}
