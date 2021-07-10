# tp_freertos
TP FreeRTOS on STM32F1VLDiscovery board or STM32F103 (update the LD script accordingly)

Assumes the Long Term Supported version of FreeRTOS as available at https://github.com/FreeRTOS/FreeRTOS-LTS 
(remember to ``git clone --recursive``) is located at the same level than this set of examples: tune the Makefiles
accordingly for the header files and libraries to reach this directory. Former versions of FreeRTOSv9.0.0 or 
FreeRTOSv10.0.0 can be used as well: in that case tune the parameters of Makefile.v9 accordingly).

Using libopencm3 to provide system calls required by FreeRTOS running on the STM32 microcontroller (instead
of ST Microelectronics' libstm32) is described at http://www.jiggerjuice.info/electronics/projects/arm/freertos-stm32f103-port.html 
and was used in this set of demonstrations.

Tested with Andre Beckus' port of qemu to the stm32: compile examples
for the STM32F103 (MD) device.
