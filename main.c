#include "stm32f4xx.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "inttypes.h"
#include <time.h>


void vBlinkTaskFunction(void* pvParameters)
{
	while(1)
	{
		for (int m=0;m<4;m++)
		{
			GPIOA->ODR |= GPIO_ODR_ODR_5; // led on A5
			vTaskDelay(1000);
			GPIOA->ODR &= ~GPIO_ODR_ODR_5; //led off
			vTaskDelay(1000);
		}
		vTaskDelay(5000);
	}
}

void vRunningTaskFunction(void* pvParameters)
{
	while(1)
	{
		for (int m=0;m<4;m++)
		{
			for (int i=0;i<8;i++)
			{
				GPIOC->ODR = (1 << (15-i)) | (1 << (i));
				vTaskDelay(1000);
			}
		}
		vTaskDelay(5);
	}
}

//c7-c12
void InitHardware()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  //razreshenie raboti ustroistv
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOA->MODER |= GPIO_MODER_MODER5_0; //00 - input, 01 - out, 10 - alt func, 11 - analog input
}


int main ()
{

	InitHardware();
	
	xTaskCreate(vBlinkTaskFunction, "blink", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate(vRunningTaskFunction, "runnig", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	vTaskStartScheduler();

	
	while(1)
	{

	}
}