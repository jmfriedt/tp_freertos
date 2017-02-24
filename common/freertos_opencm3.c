//http://www.jiggerjuice.info/electronics/projects/arm/freertos-stm32f103-port.html
//Port of FreeRTOS to libopencm3 STM32F103
//FreeRTOS is a popular open source real time operating system for microcontrollers that has been ported for use with a very large number of devices and development systems. This miniproject adapts the CORTEX_STM32F103_Primer_GCC demo to the libopencm3 environment used in many of the ARM projects on this site.
//
//The only significant change to make is to redirect the sv, pendsv and systick libopencm3 interrupt handlers to those used by FreeRTOS:

void sv_call_handler(void)
{
      vPortSVCHandler();
}
 void pend_sv_handler(void)
{
      xPortPendSVHandler();
}
void sys_tick_handler(void)
{
      xPortSysTickHandler();
} 
