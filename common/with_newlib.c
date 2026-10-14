// picolibc
#include "with_newlib.h"

#ifdef with_picolib
static FILE _uart_stream =
        FDEV_SETUP_STREAM(uart_fdev_write, uart_fdev_read, _FDEV_SETUP_RW);
FILE *uart_stream = &_uart_stream;
stdout = uart_stream;
stdin = uart_stream;
/* Fdev compatible wrappers */
static int uart_fdev_write(char c, FILE *stream) {
        uart_putchar(c);
        return 0;
}
static int uart_fdev_read(FILE *stream) {
        return uart_getchar();
}
#endif
