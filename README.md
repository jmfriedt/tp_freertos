# tp_freertos
TP FreeRTOS on STM32F1VLDiscovery board or STM32F103 (update the LD script accordingly)

Assumes the FreeRTOSv9.0.0 or FreeRTOSv10.0.0 directory to be next to the examples (update the directory
path in Makefile.v9 accordingly)

see http://www.jiggerjuice.info/electronics/projects/arm/freertos-stm32f103-port.html 
for using libopencm3 instead of libstm32.

Tested with Andre Beckus' port of qemu to the stm32: compile examples
for the STM32F103 (MD) device.
