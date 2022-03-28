Cross correlation result by computing the inverse Fourier transform of the product
of the Fourier transforms of the signal and the transmitted code. Depends on CMSIS or
KISSFFT: tune the Makefile constants to define the directory holding the FreeRTOS source 
tree and CMSIS.

<img src="xcorr_fft.png">

On this chart code B is code 1 which is the one transmitted in the noisy measurement, and
code A is code 2 which is orthogonal to code 1 and hence does not exhibit a correlation peak.

Can be tested with the custom STM32 version of QEMU provided by Andre Beckus at
https://github.com/beckus/qemu_stm32 by running ``make qemu``.

Rather than compiling the KISSFFT library as would be achieved with

```
make FIXED_POINT=32 KISSFFT_DATATYPE=int32_t KISSFFT_TOOLS=0 KISSFFT_STATIC=1 CC="arm-none-eabi-gcc -mthumb 
-mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd" KISSFFT_USE_ALLOCA=1 all
```
we select the appropriate ``kiss_fft.c`` file and provide the relevant arguments in the ``Makefile`` for 
selecting data types.
