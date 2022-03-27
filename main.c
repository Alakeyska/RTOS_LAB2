#include "stm32f4xx.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"
#include "queue.h"

//xSemaphoreHandle x_INTERRUPT;

SemaphoreHandle_t x_INTERRUPT;
xQueueHandle queue;

void exti_init()
{
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; 
  EXTI->RTSR = EXTI_RTSR_TR13;                
  //EXTI->PR = EXTI_PR_PR13;                      
  EXTI->IMR = EXTI_IMR_MR13;                  
  NVIC_EnableIRQ(EXTI15_10_IRQn);              
  NVIC_SetPriority(EXTI15_10_IRQn,5);
  __enable_irq(); 
}


void EXTI15_10_IRQHandler(void)
{
	BaseType_t needCS = pdFALSE;
	EXTI->PR = EXTI_PR_PR13;
	xSemaphoreGiveFromISR(x_INTERRUPT, &needCS);
	if(needCS == pdTRUE)
	{
		portYIELD_FROM_ISR(needCS);
	}
}

void IST(void *pvParams)
{
  int mode = 0;
	while(1)
	{
		if((xSemaphoreTake(x_INTERRUPT, portMAX_DELAY)) == pdPASS)
			{
				  mode=!mode;
				  xQueueSendToFrontFromISR(queue,&mode,0);
			}
	}
}



void vTask1(void *pvParams)
{
	int mode = 0;
	while(1)
	{
		xQueueReceive(queue,&mode,0);
		if(mode)
		{
			for(uint8_t i = 0; i < 8; i++)
			{
				GPIOA->ODR = (1 << (15-i)) | (1 << (i));
				vTaskDelay(400);
			}
		}
		else
		{
			for(uint8_t i = 8; i > 0; i--)
			{
				GPIOA->ODR = (1 << (15-i)) | (1 << (i));
				vTaskDelay(400);
			}
		}
	}
}
	

int main()
{
	RCC->AHB1ENR |=RCC_AHB1ENR_GPIOAEN; //gpio is on
	RCC->AHB1ENR |=RCC_AHB1ENR_GPIOCEN;
	GPIOA->MODER |= GPIO_MODER_MODER5_0; //5 OUTPUT
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	exti_init();
	queue = xQueueCreate(16,sizeof(int));
	x_INTERRUPT = xSemaphoreCreateBinary();
	if(x_INTERRUPT != NULL)
	{
		xTaskCreate(IST, "interrupt", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
		xTaskCreate(vTask1, "flash", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

		vTaskStartScheduler();
	}
		
	
	while(1)
	{

	}
}
