all: usart_cm3.bin 

# basic configuration
# CFLAGS= -g -Wall -MD -I$(HOME)/libopencm3/include -mthumb -DSTM32F1 -DSTM32F10X_MD \
	-mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd  # STM32F103 = Medium Device
CFLAGS= -g -Wall -MD -I$(HOME)/libopencm3/include -mthumb -DSTM32F1 -DSTM32F10X_LD_VL \
	-mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd  # STM32F100 = Low Device

LDFLAGS = -lopencm3_stm32f1 \
          -T stm32_cm3.ld  \
          -L$(HOME)/libopencm3/lib --static \
          -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd  \
          -Wl,--gc-sections -mthumb

# uncomment to use newlib 
#LDFLAGS+= -lm -lnosys 
#CFLAGS +=-Davec_newlib

syscalls.o: syscalls.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -c $^

main.o: main.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -o main.o -c $^

usart_cm3.o: usart_cm3.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -o usart_cm3.o -c $^

affiche.o: affiche.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -c $^

adc_cm3.o: adc_cm3.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -c $^

gpio_cm3.o: gpio_cm3.c
	arm-none-eabi-gcc -fno-common $(CFLAGS) -c $^

usart_cm3.bin: main.o usart_cm3.o syscalls.o adc_cm3.o gpio_cm3.o affiche.o
	arm-none-eabi-gcc -o usart_cm3.elf syscalls.o usart_cm3.o main.o adc_cm3.o gpio_cm3.o affiche.o $(LDFLAGS) -nostartfiles 
	arm-none-eabi-objcopy -O binary usart_cm3.elf usart_cm3.bin	

exec: usart_cm3.bin
	../../qemu/qemu_stm32/arm-softmmu/qemu-system-arm  -M stm32-p103 -serial stdio -serial stdio -kernel usart_cm3.bin -nographic

exec_gdb: usart_cm3.bin
	../../qemu/qemu_stm32/arm-softmmu/qemu-system-arm  -M stm32-p103 -serial stdio -serial stdio -kernel usart_cm3.bin -s -S -nographic

flash: usart_cm3.bin
	st-flash write usart_cm3.bin 0x8000000
#	stm32flash.sh -w usart_cm3.bin /dev/ttyUSB0

clean:
	rm *.o *.bin *.d *.elf DAC*
