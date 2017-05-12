#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f4/memorymap.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/flash.h> // definitions du timer


#include "stm32_initialisation.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "dds.h"
#include "mesure.h"

static void vLedsFlash(void* dummy);
static void shutdownLeds(void);
static void vGetCommand(void* dummy);
static void vDoMesure(void* dummy);

QueueHandle_t xQueueParams;
static TaskHandle_t xTaskLeds, xTaskDo, xTaskOver;

struct AMessage
{
	uint32_t startFreq, endFreq, stepFreq, delayFreq;
	uint8_t moyFreq;
};


int main (void)
{
	core_clock_setup();
	clock_setup();
	init_Gpio();
	init_Timer(70);
	init_DDS();
	init_Mesure();
	init_Usart();
	
	xQueueParams = xQueueCreate(1, sizeof(struct AMessage));
	
	if(xQueueParams == NULL) goto hell;

	
	if (!(pdPASS == xTaskCreate(vLedsFlash,(char*) "LedFlash",128,NULL,10, &xTaskLeds ))) goto hell;
	if (!(pdPASS == xTaskCreate(vGetCommand,(char*)"GetCommand",128,NULL,10, &xTaskOver ))) goto hell;
	if (!(pdPASS == xTaskCreate(vDoMesure,(char*) "DoMesure",128,NULL,10, &xTaskDo ))) goto hell;
	vTaskStartScheduler();
	
	
hell: 
	while(1)
	{
		gpio_set(GPIOA, Led_Rouge);
		vTaskDelay(200/portTICK_RATE_MS);
		gpio_clear(GPIOA, Led_Rouge);
		vTaskDelay(200/portTICK_RATE_MS);
	}
	
	return 0;
}

void vLedsFlash(void* dummy)
{
	uint8_t delay=100;
	
	while(1)
	{
		gpio_set(GPIOA, Led_Verte);
		vTaskDelay(delay/portTICK_RATE_MS);
		gpio_clear(GPIOA, Led_Verte);
		gpio_set(GPIOA, Led_Jaune);
		vTaskDelay(delay/portTICK_RATE_MS);
		gpio_clear(GPIOA, Led_Jaune);
		gpio_set(GPIOA, Led_Rouge);
		vTaskDelay(delay/portTICK_RATE_MS);
		gpio_clear(GPIOA, Led_Rouge);
		gpio_set(GPIOA, Led_Jaune);
		vTaskDelay(delay/portTICK_RATE_MS);
		gpio_clear(GPIOA, Led_Jaune);
	}
}

void shutdownLeds(void)
{
	gpio_clear(GPIOA, Led_Verte | Led_Jaune | Led_Rouge);
}

void vGetCommand(void* dummy)
{
	char buffer[100], newCar, cmdIndex=0;
	int caracs=0, curCar;
	uint64_t temp=0;
	struct AMessage pxSendMessage;
	putString("Enter the following command:\r\n");
	putString("StartFreq(Hz)/EndFreq(Hz)/FreqStep(Hz)/FreqDelay(ms)/Average(n)\r\n:");
	while(1)
	{
		newCar=getCharBlocking();
		if(newCar>='/' && newCar<='9')//On récupère les caractères
		{
			buffer[caracs]=newCar;
			putChar(newCar);
			caracs++;
		}
		else if(newCar=='\r')//On vient de valider la commande
		{
			for(curCar=0; curCar<caracs; curCar++)//On parcours le tampon
			{
				if(buffer[curCar]=='/')//C'est un séparateur
				{
					if(cmdIndex<4)//On lit un max de 5 arguments
					{
						//On enregistre l'argument et on passe au suivant
						//Wclk=(Fout*2^28)/70e6=(Fout*2^21)/546875
						if(cmdIndex<3)
							temp=(temp<<21)/546875;
						
						if(cmdIndex==0)
							pxSendMessage.startFreq=(uint32_t)temp;
						else if(cmdIndex==1)
							pxSendMessage.endFreq=(uint32_t)temp;
						else if(cmdIndex==2)
							pxSendMessage.stepFreq=(uint32_t)temp;
						else if(cmdIndex==3)
							pxSendMessage.delayFreq=(uint32_t)temp;
						temp=0;
						cmdIndex++;
					}
					else//Sinon la commande n'est pas valide
					{
						cmdIndex=0;
						break;
					}
				}
				else//C'est un chiffre
				{
					temp=temp*10+(buffer[curCar]-'0');
				}
			}
			
			if(cmdIndex==4 && temp>0)//Si le reste est passé, on enregistre le dernier argument
			{
				pxSendMessage.moyFreq=(uint8_t)temp;
				cmdIndex++;
			}
			
			if(cmdIndex>4)//On a tout rempli
			{
				xQueueSendToBack(xQueueParams, &pxSendMessage, 100);
				vTaskSuspend(xTaskLeds);//On éteint les leds
				shutdownLeds();
				putString("\r\n");

				xTaskNotifyGive(xTaskDo);//On lance le scan
				
				ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//On attend d'être débloqué
				putString("Enter the following command:\r\n");
				putString("StartFreq(Hz)/EndFreq(Hz)/FreqStep(Hz)/FreqDelay(ms)/Average(n)0\r\n:");
			}
			else
			{
				putString("Enter the following command:\r\n");
				putString("StartFreq(Hz)/EndFreq(Hz)/FreqStep(Hz)/FreqDelay(ms)/Average(n)1\r\n:");
			}	
			
			caracs=0;
			cmdIndex=0;
			temp=0;
			pxSendMessage.startFreq=0;
			pxSendMessage.endFreq=0;
			pxSendMessage.stepFreq=0;
			pxSendMessage.delayFreq=0;
			pxSendMessage.moyFreq=0;
		}
	}
}

void vDoMesure(void* dummy)
{
	//30678337->8MHz
	struct AMessage pxRecvMessage;
	uint32_t frequency, somMoyMag, somMoyPha;
	uint8_t moy;
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//On attend d'être débloqué
		if(xQueueReceive(xQueueParams, &pxRecvMessage, 100))
		{
		  for (frequency=pxRecvMessage.startFreq;frequency<=pxRecvMessage.endFreq;
                       frequency+=pxRecvMessage.stepFreq)
		      {	
			set_FreqWord(frequency);
			vTaskDelay(pxRecvMessage.delayFreq/portTICK_RATE_MS);
			somMoyMag=0;
			somMoyPha=0;
			for(moy=0; moy<pxRecvMessage.moyFreq; moy++)
			{
				somMoyMag+=read_Mag();
				somMoyPha+=read_Phase();
			}
			putHexa16((uint16_t)(somMoyMag/pxRecvMessage.moyFreq));
			putChar('/');
			putHexa16((uint16_t)(somMoyPha/pxRecvMessage.moyFreq));
			putChar('/');
			putHexa32(frequency); //putDec32(frequency);
			putChar('\r');
			putChar('\n');
		      }	
		}	
		
		putString("END\r\n");
		vTaskResume(xTaskLeds);
		xTaskNotifyGive(xTaskOver);//On lance l'interface
		if(getChar()==3)//Si on reçoit un ordre d'arrêt
		{
			vTaskResume(xTaskLeds);
			xTaskNotifyGive(xTaskOver);//On lance l'interface
		}
	}
}
