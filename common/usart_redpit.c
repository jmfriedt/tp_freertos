#include <stdint.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <ps7_init.h>

#define XUARTPS_CR_TXRST	0x00000002U  /**< TX logic reset */
#define XUARTPS_CR_RXRST	0x00000001U  /**< RX logic reset */

#define XUARTPS_CR_OFFSET       0x0000U  /**< Control Register [8:0] */
#define XUARTPS_MR_OFFSET       0x0004U  /**< Mode Register [9:0] */
#define XUARTPS_BAUDGEN_OFFSET  0x0018U  /**< Baud Rate Generator [15:0] */
#define XUARTPS_BAUDDIV_OFFSET  0x0034U  /**< Baud Rate Divider [7:0] */
#define XUARTPS_FIFO_OFFSET     0x0030U  /**< FIFO [7:0] */
#define XUARTPS_SR_OFFSET       0x002CU  /**< Channel Status [14:0] */
#define XPS_UART1_BASEADDR      0xE0001000U
#define XPS_UART0_BASEADDR      0xE0000000U

#define XUARTPS_MR_CHMODE_NORM		0x00000000U /**< Normal mode */
#define XUARTPS_MR_STOPMODE_1_BIT	0x00000000U /**< 1 stop bit */
#define XUARTPS_MR_PARITY_NONE		0x00000020U /**< No parity mode */
#define XUARTPS_MR_CHARLEN_8_BIT	0x00000000U /**< 8 bits data */
#define XUARTPS_MR_CLKSEL			0x00000001U /**< Input clock selection */

#define XUARTPS_SR_TNFUL	0x00004000U /**< TX FIFO Nearly Full Status */
#define XUARTPS_SR_TACTIVE	0x00000800U /**< TX active */
#define XUARTPS_SR_RXEMPTY	0x00000002U /**< RX FIFO empty */

#define XUARTPS_CR_TX_DIS	0x00000020U  /**< TX disabled. */
#define XUARTPS_CR_TX_EN	0x00000010U  /**< TX enabled */
#define XUARTPS_CR_RX_DIS	0x00000008U  /**< RX disabled. */
#define XUARTPS_CR_RX_EN	0x00000004U  /**< RX enabled */

#define POINTER_TO_REGISTER(REG)  ( *((volatile uint32_t*)(REG)))

#define UART_BASE      XPS_UART0_BASEADDR
#define UART_CTRL      POINTER_TO_REGISTER(UART_BASE + XUARTPS_CR_OFFSET)      // Control Register
#define UART_MODE      POINTER_TO_REGISTER(UART_BASE + XUARTPS_MR_OFFSET)      // Mode Register

#define UART_BAUD_GEN  POINTER_TO_REGISTER(UART_BASE + XUARTPS_BAUDGEN_OFFSET) // Baud Rate Generator "CD"
#define UART_BAUD_DIV  POINTER_TO_REGISTER(UART_BASE + XUARTPS_BAUDDIV_OFFSET) // Baud Rate Divider "BDIV"
#define UART_FIFO      POINTER_TO_REGISTER(UART_BASE + XUARTPS_FIFO_OFFSET)    // FIFO
#define UART_STATUS    POINTER_TO_REGISTER(UART_BASE + XUARTPS_SR_OFFSET)      // Channel Status

#define MIO_PIN07_CTRL       *((uint32_t *)0xF800071C)
#define MIO_PIN07_CTRL_GPIO0 0x00003200

#define GPIO_DIRM0           *((uint32_t *)0xE000A204)
#define GPIO_OEN0            *((uint32_t *)0xE000A208)
#define GPIO_DATA0           *((uint32_t *)0xE000A040)

#define GPIO0_PIN7           0x80

void uart_send(char c) {
  while (UART_STATUS & XUARTPS_SR_TNFUL);
  UART_FIFO = c;
  while (UART_STATUS & XUARTPS_SR_TACTIVE);
}

char uart_recv() {
  if ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY)
    return 0;
  return UART_FIFO;
}

char uart_recv_blocking() {
  while ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY);
  return UART_FIFO;
}

void uart_setup(void) {
  uint32_t r = 0; // Temporary value variable
  r = UART_CTRL;
  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
  UART_CTRL = r;

  UART_MODE = 0;
  UART_MODE &= ~XUARTPS_MR_CLKSEL; // Clear "Input clock selection" - 0: clock source is uart_ref_clk
  UART_MODE |= XUARTPS_MR_CHARLEN_8_BIT; 	// Set "8 bits data"
  UART_MODE |= XUARTPS_MR_PARITY_NONE; 	// Set "No parity mode"
  UART_MODE |= XUARTPS_MR_STOPMODE_1_BIT; // Set "1 stop bit"
  UART_MODE |= XUARTPS_MR_CHMODE_NORM; 	// Set "Normal mode"

  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
  UART_BAUD_DIV = 6; // ("BDIV")
  UART_BAUD_GEN = 124; // ("CD")
  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps

  UART_CTRL |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

  r = UART_CTRL;
  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
  UART_CTRL = r;
}

void gpio_setup() {
  MIO_PIN07_CTRL = MIO_PIN07_CTRL_GPIO0;
  GPIO_DIRM0 |= GPIO0_PIN7;
  GPIO_OEN0  |= GPIO0_PIN7;
}

static inline void led_toggle() {
}

void clock_setup(void)
{
}

void Usart1_Init(void)
{ ps7_init();   // see FreeRTOS/FreeRTOS/Demo/CORTEX_A9_Zynq_ZC702/RTOSDemo/src/platform.c
  gpio_setup();
  uart_setup();
}

void Led_Init(void)
{
}

void Led_Hi1(void) { GPIO_DATA0 |= GPIO0_PIN7;}
void Led_Lo1(void) { GPIO_DATA0 &= (~GPIO0_PIN7);}
void Led_Hi2(void) {}
void Led_Lo2(void) {}

void uart_putc(char c) {uart_send(c);}

/* Writes a zero teminated string over the serial line*/
void uart_puts(char *c) {while(*c!=0) uart_putc(*(c++));}
