/*
 *
 *	This project is to use the Sleep on Exit mode after exiting an ISR.
 *	We set the Timer 3 to interrupt every 10ms and send some data over the USART2,
 *	and after that, MCU will enter in sleep mode
 */

#include "stm32f4xx.h"
#include "main.h"
#include <string.h>

/*
 * Functions prototypes
 */
void SystemCoreClockConfig(uint8_t clock_freq);
void Error_handler(void);
void GPIO_Init(void);
void TIMER3_Init(void);
void UART2_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

TIM_HandleTypeDef htimer3;
UART_HandleTypeDef huart2; //Handle of UART 2
extern uint8_t some_data[];


int main(){


	/* Basic initialization  */
	HAL_Init();

	//SystemCoreClockConfig(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	UART2_Init();

	TIMER3_Init();

	//SCB->SCR |= ( 1 << 1); //Set SLEEOPONEXIT bit to 1
	HAL_PWR_EnableSleepOnExit();
	/*
	 * Start with fresh status register to avoid false interrupts
	 */
	TIM3->SR = 0;

	//Start the timer in interrupt mode
	HAL_TIM_Base_Start_IT(&htimer3);

	while(1);

	return 0;
}


void TIMER3_Init(void){

	//High level initialization of the TIMER 3 for 10ms if using HSI
	htimer3.Instance = TIM3;
	htimer3.Init.Period = 4999;
	htimer3.Init.Prescaler = 39 - 1;

	if( HAL_TIM_Base_Init(&htimer3) != HAL_OK )
		{
			Error_handler();
		}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if ( HAL_UART_Transmit(&huart2, (uint8_t*)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY) != HAL_OK){
			 Error_handler();
		 }
}

/*
 * You can use it when you need an specific clock
 * By default will be internal clock
 */
void SystemCoreClockConfig(uint8_t clock_freq){

	RCC_OscInitTypeDef Osc_Init;
	RCC_ClkInitTypeDef Clock_Init;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSE;
	Osc_Init.HSIState = RCC_HSI_ON;
	Osc_Init.LSEState = RCC_LSE_ON;
	Osc_Init.HSEState = RCC_HSE_ON;
	Osc_Init.HSICalibrationValue = 16;
	Osc_Init.PLL.PLLState = RCC_PLL_ON;
	Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSI;

	switch(clock_freq){
	  case SYS_CLOCK_FREQ_50_MHZ:
			  Osc_Init.PLL.PLLM = 8;
			  Osc_Init.PLL.PLLN = 50;
			  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			  Osc_Init.PLL.PLLQ = 2;
			  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
			  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
		     break;

		  case SYS_CLOCK_FREQ_84_MHZ:
			  Osc_Init.PLL.PLLM = 8;
			  Osc_Init.PLL.PLLN = 84;
			  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			  Osc_Init.PLL.PLLQ = 2;
			  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
			  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
		     break;

		  case SYS_CLOCK_FREQ_120_MHZ:
			  Osc_Init.PLL.PLLM = 8;
			  Osc_Init.PLL.PLLN = 120;
			  Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			  Osc_Init.PLL.PLLQ = 2;
			  Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
			  Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			  Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			  Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
			  Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
		     break;

		  default:
		   return ;
		 }

			if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)
		{
				Error_handler();
		}



		if (HAL_RCC_ClockConfig(&Clock_Init, FLASH_LATENCY_2) != HAL_OK)
		{
			Error_handler();
		}


		/*Configure the systick timer interrupt frequency (for every 1 ms) */
		uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
		HAL_SYSTICK_Config(hclk_freq/1000);

		/**Configure the Systick
		*/
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		/* SysTick_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void UART2_Init(void){

	//1. Linking handle struct to base address
	huart2.Instance = USART2;

	//2. Init the handle variable
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;

	//3. Use the API to initialize
	if(HAL_UART_Init(&huart2) != HAL_OK){

		//There is a problem
		Error_handler();
	}
}


void GPIO_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}

void Error_handler(void){
	while(1);
}
