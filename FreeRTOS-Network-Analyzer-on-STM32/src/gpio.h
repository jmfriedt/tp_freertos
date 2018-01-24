//Port A
#define NSS GPIO4
#define SCK GPIO5
#define MISO GPIO6
#define MOSI GPIO7
#define TIM1_CH1 GPIO8
#define RSRX GPIO9
#define RSTX GPIO10
#define Led_Rouge GPIO11
#define Led_Jaune GPIO12
#define Led_Verte GPIO13
#define SLEEP GPIO14
#define RST_DDS GPIO15

//Port B
#define PHASE GPIO0
#define MAGNITUDE GPIO1

//Port C
#define ENA_DET GPIO10



void init_Gpio(void);
void delay(uint64_t delay);


