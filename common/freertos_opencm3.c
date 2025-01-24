#include "stdint.h"
void vPortSVCHandler(void);
void xPortPendSVHandler(void);
void xPortSysTickHandler(void);

//http://www.jiggerjuice.info/electronics/projects/arm/freertos-stm32f103-port.html
//Port of FreeRTOS to libopencm3 STM32F103
//FreeRTOS is a popular open source real time operating system for microcontrollers that has been ported for use with a very large number of devices and development systems. This miniproject adapts the CORTEX_STM32F103_Primer_GCC demo to the libopencm3 environment used in many of the ARM projects on this site.
//
//The only significant change to make is to redirect the sv, pendsv and systick libopencm3 interrupt handlers to those used by FreeRTOS:

/* https://www.freertos.org/FAQHelp.html 
#define vPortSVCHandler     sv_call_handler
#define xPortPendSVHandler  pend_sv_handler
#define xPortSysTickHandler sys_tick_handler
*/

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

/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
static void hard_fault_handler( void ) __attribute__( ( naked ) );

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void hard_fault_handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}
