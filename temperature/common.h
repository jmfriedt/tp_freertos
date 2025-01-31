// hardware dependent function declarations
void Led_Init(void);
void Led_Hi1(void);
void Led_Lo1(void);
void Led_Hi2(void);
void Led_Lo2(void);
void Usart1_Init(void);
void uart_putc(char c);
void uart_puts(char *c);
void adc_setup(void);
unsigned short read_adc_naiive(unsigned char);
