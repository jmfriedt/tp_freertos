NAME=main
SRCDIR=./src
OUTPUTDIR=./output
LDDIR=./ld
FREERTOSDIR=/home/jmfriedt/enseignement/ufr/platforms/stm32/F1/FreeRTOSv9.0.0/FreeRTOS/Source/
DEV=DSTM32F4
MCPU=cortex-m4

CFLAGS=	-c \
		-fno-common \
		-ffunction-sections \
		-fdata-sections \
		-O1 \
		-mcpu=$(MCPU) \
		-mthumb \
		-g3 \
		-$(DEV) \
		-MD \
		-hard-float -mfloat-abi=hard -mfpu=fpv4-sp-d16
		
LDFLAGS=-lopencm3_stm32f4 --static \
		-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
		-T $(LDDIR)/stm32f4.ld -nostartfiles -Wl,--gc-sections -mthumb \
		-MD -mcpu=$(MCPU) -hard-float -mfloat-abi=hard -mfpu=fpv4-sp-d16

OBJS=	$(SRCDIR)/main.o \
		$(SRCDIR)/timer.o \
		$(SRCDIR)/gpio.o \
		$(SRCDIR)/usart.o \
		$(SRCDIR)/stm32_initialisation.o \
		$(SRCDIR)/dds.o \
		$(SRCDIR)/mesure.o \
		$(FREERTOSDIR)/timers.o \
		$(FREERTOSDIR)/portable/MemMang/heap_2.o \
		$(FREERTOSDIR)/portable/GCC/ARM_CM4F/port.o \
		$(FREERTOSDIR)/tasks.o \
		$(FREERTOSDIR)/queue.o \
		$(FREERTOSDIR)/croutine.o \
		$(FREERTOSDIR)/list.o
		
		
INCLUDES=	-I$(SRCDIR) \
			-I$(FREERTOSDIR)/include/ \
			-I$(FREERTOSDIR)/portable/GCC/ARM_CM4F/ 


##################################
# Compilation toolchain

all : $(OUTPUTDIR)/$(NAME).bin
	

$(OUTPUTDIR)/$(NAME).bin: $(OUTPUTDIR)/$(NAME).elf
	arm-none-eabi-objcopy -Obinary $(OUTPUTDIR)/$(NAME).elf $(OUTPUTDIR)/$(NAME).bin
	
$(OUTPUTDIR)/$(NAME).elf: $(OBJS) 
	arm-none-eabi-gcc -o $(OUTPUTDIR)/$(NAME).elf $(OBJS) $(LDFLAGS)

%.o: %.c
	arm-none-eabi-gcc $(INCLUDES) $(CFLAGS) $< -o $*.o

%.o: %.s
	arm-none-eabi-gcc $(INCLUDES) $(CFLAGS) $< -o $*.o

##################################
# Execution command

flash: $(OUTPUTDIR)/$(NAME).bin
	stm32flash.sh -w $(OUTPUTDIR)/$(NAME).bin /dev/ttyUSB0

##################################
# Clean command 

clean:
	@echo "Removing files..."
	-find . -name '*.o'   | xargs rm
	-find . -name '*.d'   | xargs rm
	-find . -name '*.elf' | xargs rm
	-find . -name '*.bin' | xargs rm
	-find $(FREERTOSDIR) -name '*.o' | xargs rm
	-find $(FREERTOSDIR)/portable/MemMang -name '*.o' | xargs rm
	-find $(FREERTOSDIR)/portable/GCC/ARM_CM4F -name '*.o' | xargs rm
