LIBOPENCM3_LOCATION:=/home/jmfriedt/libopencm3

CFLAGS =  -Wall -Os
CFLAGS += -mthumb -mhard-float -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -DLM4F -DARM_MATH_CM4 -D__FPU_PRESENT=1
CFLAGS += --static -nostartfiles -I../../kissfft
CFLAGS += -g3 -c -fno-common -ffunction-sections -fdata-sections -DFIXED_POINT=32 -DKISSFFT_DATATYPE=int32_t 
LDFLAGS = -mhard-float -mfloat-abi=hard -mfpu=fpv4-sp-d16 -L$(LIBOPENCM3_LOCATION)/lib -L../kissfft/
LDFLAGS +=-lopencm3_lm4f -lkissfft-int32_t -lm

LDFLAGS	+= --static \
	-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
	-L../../ld -Tstellaris.ld -nostartfiles -Wl,--gc-sections -mthumb \
	-mcpu=cortex-m4 

baremetal.bin: baremetal.elf
	arm-none-eabi-objcopy -Obinary baremetal.elf baremetal.bin
	
baremetal.elf: baremetal.o
	arm-none-eabi-gcc -o baremetal.elf baremetal.o ../../common/usart_stellaris.o -L../../kissfft $(LDFLAGS)

baremetal.o: baremetal.c
	arm-none-eabi-gcc $(CFLAGS) -c baremetal.c

qemu: baremetal.bin
	qemu-system-arm -cpu cortex-m4 -machine lm3s6965evb -nographic -vga none -net none -serial mon:stdio -kernel baremetal.bin # -s -S for gdb

clean:
	rm *.o baremetal.elf baremetal.bin
