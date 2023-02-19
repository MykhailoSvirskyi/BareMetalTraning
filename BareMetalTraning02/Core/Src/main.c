/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define led_number 4
#define DEBOUNCE 50 // button debounce timing

const uint16_t led_num = led_number; // pcs of used LED;
uint32_t perioD = 100;				 // variable blink period
uint32_t impulS = 50;				 // turn on time
uint32_t sds;						 // startup led pin delay
uint32_t quantum = 1;				 // check every this time are state of the port needs to be changed
typedef struct LED
{
	uint32_t period;	  // blink period
	uint32_t impuls;	  // on time
	uint32_t start_delay; // startup delay
	uint16_t pin;		  // blink pin
	GPIO_TypeDef *port;	  // blink port
} led_t;

led_t LED[led_number];

uint32_t lastTick = 0; // tick counter

uint32_t last_time = 0;
uint32_t time = 0;
uint16_t BLINK_MODE = 1;
bool ON_OFF = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*interrupt button actions*/

	uint32_t time = HAL_GetTick();

	if (time - last_time > DEBOUNCE)
	{ // button debounce
		/*true press button*/
		switch (GPIO_Pin)
		{
		case sw1_next_Pin: // blink mode change
			if (BLINK_MODE >= 3)
			{
				BLINK_MODE = 1;
			}
			else
			{
				BLINK_MODE++;
			}
			break;

		case sw2_on_off_Pin: // off/on all led`s
			if (ON_OFF == true)
			{
				ON_OFF = false;
			}
			else
			{
				ON_OFF = true;
			}
			break;
		case sw3_back_Pin: // blink mode change
			if (BLINK_MODE > 1)
			{
				BLINK_MODE--;
			}
			else
			{
				BLINK_MODE = 3;
			}
			break;
		case sw4_speed_down_Pin:
			if (perioD >= 200)
			{
				perioD -= 100;
			}

			break;
		case sw5_speed_up_Pin:
			if (perioD < 800)
			{
				perioD += 100;
			}

			break;
		default:
			Error_Handler();
		}
		last_time = time;
	}

	/*setup blink parameter*/
	impulS = perioD / 2;

	switch (BLINK_MODE)
	{
	case 1:
		sds = perioD / 10;
		break;

	case 2:
		sds = perioD / 4;
		break;

	case 3:
		sds = perioD / 2;
		break;

	default:
		Error_Handler();
	}
	for (uint16_t i = 0; i < led_num; i++)
	{
		LED[i].period = perioD; // perioD;
		LED[i].impuls = impulS; // impulS;
		LED[i].port = GPIOD;
	}

	/*start_delay_step*/

	LED[0].start_delay = sds;
	LED[1].start_delay = sds * 2;
	LED[2].start_delay = sds * 3;
	LED[3].start_delay = sds * 4;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	LED[0].pin = GPIO_PIN_12;
	LED[1].pin = GPIO_PIN_13;
	LED[2].pin = GPIO_PIN_14;
	LED[3].pin = GPIO_PIN_15;
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		if (HAL_GetTick() - lastTick >= quantum)
		{ // check status every (ms) quantum

			for (uint32_t j = 0; j < led_num; j++)
			{
				if (lastTick >= LED[j].start_delay) // check if start time come
				{
					if (ON_OFF == true)
					{
						if (((lastTick + LED[j].period - LED[j].start_delay) % LED[j].period) == 0) // if time turn on pin
						{
							HAL_GPIO_WritePin(LED[j].port, LED[j].pin, GPIO_PIN_SET);
						}
					}
					else
					{
						HAL_GPIO_WritePin(LED[j].port, LED[j].pin, GPIO_PIN_RESET);
					}
					if (((lastTick + LED[j].period - LED[j].start_delay - LED[j].impuls) % LED[j].period) == 0) // if time turn off pin
					{
						HAL_GPIO_WritePin(LED[j].port, LED[j].pin, GPIO_PIN_RESET);
					}
				}
			}
			lastTick = HAL_GetTick();
		}

		/* USER CODE BEGIN 3 */

		/* USER CODE END 3 */
	}
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
