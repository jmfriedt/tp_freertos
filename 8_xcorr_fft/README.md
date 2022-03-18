Cross correlation result by computing the inverse Fourier transform of the product
of the Fourier transforms of the signal and the transmitted code. Depends on CMSIS:
tune the Makefile constants to define the directory holding the FreeRTOS source tree
and CMSIS.

<img src="xcorr_fft.png">

Can be tested with the ARM version of QEMU emulating the Texas Instruments Stellaris
LM3S6965 Cortex M4F core by running ``make qemu`` assuming ``qemu-system-arm`` is installed
on the host computer.
