all: usart_stm32.bin
#all: usart_cm3.bin usart_stm32.bin pwm_cm3.bin

usart_cm3.bin: main_cm3.c ../common/usart_opencm3.c
	arm-none-eabi-gcc -Wall -Wextra -Wimplicit-function-declaration \
           -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
           -Wundef -Wshadow -I~/sat/bin/../arm-none-eabi/include \
           -fno-common -mthumb -mcpu=cortex-m3 -msoft-float -MD -DSTM32F1 \
           -o usart_cm3.o -c ../common/usart_opencm3.c
	arm-none-eabi-gcc -Wall -Wextra -Wimplicit-function-declaration \
           -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
           -Wundef -Wshadow -I~/sat/bin/../arm-none-eabi/include \
           -fno-common -mthumb -mcpu=cortex-m3 -msoft-float -MD -DSTM32F1 \
           -o main_cm3.o -c main_cm3.c
	arm-none-eabi-gcc -o usart_cm3.elf usart_cm3.o main_cm3.o -lopencm3_stm32f1 --static \
            -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
            -L~/sat/arm-none-eabi/lib/thumb/cortex-m3 -L~/sat/bin/../arm-none-abi/lib \
            -T./stm32vl-discovery.ld -nostartfiles -Wl,--gc-sections -mthumb \
            -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd \
            -L~/sat/arm-none-eabi/lib/thumb/cortex-m3
	arm-none-eabi-objcopy -O binary usart_cm3.elf usart_cm3.bin	

pwm_cm3.bin: pwm_cm3.c
	arm-none-eabi-gcc -Wall -Wextra -Wimplicit-function-declaration \
           -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
           -Wundef -Wshadow -I~/sat/bin/../arm-none-eabi/include \
           -fno-common -mthumb -mcpu=cortex-m3 -msoft-float -MD -DSTM32F1 \
           -o usart_cm3.o -c ../common/usart_opencm3.c
	arm-none-eabi-gcc -Wall -Wextra -Wimplicit-function-declaration \
           -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
           -Wundef -Wshadow -I~/sat/bin/../arm-none-eabi/include \
           -fno-common -mthumb -mcpu=cortex-m3 -msoft-float -MD -DSTM32F1 \
           -o pwm_cm3.o -c pwm_cm3.c
	arm-none-eabi-gcc -o pwm_cm3.elf pwm_cm3.o usart_cm3.o -lopencm3_stm32f1 --static \
            -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
            -L~/sat/arm-none-eabi/lib/thumb/cortex-m3 -L~/sat/bin/../arm-none-abi/lib \
            -T./stm32vl-discovery.ld -nostartfiles -Wl,--gc-sections -mthumb \
            -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd \
            -L~/sat/arm-none-eabi/lib/thumb/cortex-m3
	arm-none-eabi-objcopy -O binary pwm_cm3.elf pwm_cm3.bin	


#LDSCRIPT= ../ld/stm32_flash.ld
LDSCRIPT= ./stm32vl-discovery.ld

usart_stm32.bin: main_stm32.c ../common/usart_stm32.c system_stm32f10x.c
	arm-none-eabi-gcc -DSTM32F10X_LD_VL -I./ -c -fno-common -O0 -g3 -I../common -I/home/jmfriedt/sat/arm-none-eabi/include/stm32f1/ -I/home/jmfriedt/sat/arm-none-eabi/include/ -Wall -Wextra -mcpu=cortex-m3 -mthumb -I/home/jmfriedt/sat/arm-none-eabi/include/include -fno-common -mthumb -msoft-float   -c -o usart_stm32.o ../common/usart_stm32.c
	arm-none-eabi-gcc -DSTM32F10X_LD_VL -I./ -c -fno-common -O0 -g3 -I../common -I/home/jmfriedt/sat/arm-none-eabi/include/stm32f1/ -I/home/jmfriedt/sat/arm-none-eabi/include/ -Wall -Wextra -mcpu=cortex-m3 -mthumb -I/home/jmfriedt/sat/arm-none-eabi/include/include -fno-common -mthumb -msoft-float   -c -o main_stm32.o main_stm32.c
	arm-none-eabi-gcc -fno-common -O0 -g3 -Wall -Wextra -mcpu=cortex-m3 -mthumb -mthumb -msoft-float -Wl,--gc-sections -T$(LDSCRIPT)  -nostartfiles -L~/sat/arm-none-eabi/lib/thumb/cortex-m3 -o usart_stm32.elf main_stm32.o usart_stm32.o -lstm32 -lopencm3_stm32f1
	arm-none-eabi-objcopy -O binary usart_stm32.elf usart_stm32.bin	

clean:
	rm *.o *.bin *.d *.elf ../common/*.o
