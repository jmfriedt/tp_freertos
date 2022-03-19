For the KISSFFT library that was downloaded in ../../kissfft during the
``git clone --recursive``, cross-compile with

```
make FIXED_POINT=32 KISSFFT_DATATYPE=int32_t KISSFFT_TOOLS=0 KISSFFT_STATIC=1  CC="arm-none-eabi-gcc -mthumb -mhard-float -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -DLM4F" KISSFFT_USE_ALLOCA=1 all
```

This will generate the static library needed to run this Makefile, and qemu simulation is
achieved with ``make qemu``
