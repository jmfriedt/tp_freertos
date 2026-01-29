## MUTEX

Demonstration with the scheduler task swapping when UART communications last
too long, unless protected by MUTEX.

# qemu

Functional with Andre Beckus Qemu, but with the official ``qemu-system-arm``, UART
timing is not implemented for the STM32F1 emulation, only for the STM32F4.

Make sure to comment the rcc initialization functions of libopencm3 which are not correctly
implemented in qemu:

```
$ git diff lib/stm32/f4/rcc.c
diff --git a/lib/stm32/f4/rcc.c b/lib/stm32/f4/rcc.c
index 99c77fa7..83dfffd3 100644
--- a/lib/stm32/f4/rcc.c
+++ b/lib/stm32/f4/rcc.c
@@ -534,15 +534,14 @@ bool rcc_is_osc_ready(enum rcc_osc osc)
 
 void rcc_wait_for_osc_ready(enum rcc_osc osc)
 {
-       while (!rcc_is_osc_ready(osc));
+//     while (!rcc_is_osc_ready(osc));
 }
 
 void rcc_wait_for_sysclk_status(enum rcc_osc osc)
 {
        switch (osc) {
        case RCC_PLL:
-               while (((RCC_CFGR >> RCC_CFGR_SWS_SHIFT) & RCC_CFGR_SWS_MASK) !=
-                       RCC_CFGR_SWS_PLL);
+//             while (((RCC_CFGR >> RCC_CFGR_SWS_SHIFT) & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL);
                break;
        case RCC_HSE:
                while (((RCC_CFGR >> RCC_CFGR_SWS_SHIFT) & RCC_CFGR_SWS_MASK) !=
```
to avoid locking the emulator in the HSE or PLL initialization step (the same applies for 
emulating the STM32F1 on Andre Beckus qemu).
