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
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim4_pwm.h"
#define temp_high_mS1 500//period mS of red led blink if 1 error 500 equal 1Hz
#define temp_high_mS2 200//period mS of red led blink if 2 error 200 equal 2.5Hz
#define temp_high_mS3 100//period mS of red led blink if 3 error 100 equal 5Hz
#define green_led TIM_CHANNEL_1 //define led pin
#define orange_led TIM_CHANNEL_2
#define blue_led TIM_CHANNEL_4
#define red_led GPIO_PIN_14
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*convert ADC value to percent, start value end value and ADC value
if start >end ADC start from 0% else ADC start from 100% and down. if ADC value > or < range we hew a 0% or 100% */
uint8_t ADC_to_percent(uint32_t start, uint32_t end, uint32_t ADC_in) {

	if (start > end && end < ADC_in) {
		if (ADC_in >= start) {
			return (0);
		} else {
			return (100 - (((ADC_in - end) * 100) / (start - end)));
		}
	}

	if (start < ADC_in && end > start) {
		if (ADC_in >= end) {
			return (100);
		} else {
			return (((ADC_in - start) * 100) / (end - start));
		}

	}
	if(start > end && ADC_in<end)
	{return (100);}
	return 0;
}
;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t start_adc_val[3]; //start ADC range
	uint32_t end_adc_val[3];//end ADC range
	uint32_t trig_val[3];// value when start emergency citation
	uint32_t hist_percent[3]; //Hysteresis percent
	uint32_t adcValue[3] = { 0 };
	uint32_t histereis[3]; //hysteresis value
	/*internal temp sensor*/
	start_adc_val[0] = 1050;
	end_adc_val[0] = 1095;
	trig_val[0] = 1100;

	/*PORTB1 temp sensor*/
	start_adc_val[1] = 2270;
	end_adc_val[1] = 400;
	trig_val[1] = 450;

	/*PORT A3 potentiometer */
	start_adc_val[2] = 0;
	end_adc_val[2] = 4090;
	trig_val[2] = 3000;

	/*	setup hysteresis*/
	hist_percent[0] = 10;
	hist_percent[1] = 1;
	hist_percent[2] = 1;

	histereis[0] = ((end_adc_val[0] - start_adc_val[0]) / 100)* hist_percent[0];
	histereis[1] = ((start_adc_val[1] - end_adc_val[1]) / 100)* hist_percent[1];
	histereis[2] = ((end_adc_val[2] - start_adc_val[2]) / 100)* hist_percent[2];

	uint8_t RedLedBlink; //red led blink mode

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM4_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
	_Tim_init();

	volatile HAL_StatusTypeDef adcPoolResult1, adcPoolResult2, adcPoolResult3;
	uint32_t lastTick = 0, maxTick = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
/*		initialize and read ADC*/
		HAL_ADC_Start(&hadc1);
		HAL_ADC_Start(&hadc2);
		HAL_ADC_Start(&hadc3);
		adcPoolResult1 = HAL_ADC_PollForConversion(&hadc1, 100);
		adcPoolResult2 = HAL_ADC_PollForConversion(&hadc2, 100);
		adcPoolResult3 = HAL_ADC_PollForConversion(&hadc3, 100);
		if (adcPoolResult1 == HAL_OK) {
			adcValue[0] = HAL_ADC_GetValue(&hadc1);
		}
		if (adcPoolResult2 == HAL_OK) {
			adcValue[1] = HAL_ADC_GetValue(&hadc2);
		}
		if (adcPoolResult3 == HAL_OK) {
			adcValue[2] = HAL_ADC_GetValue(&hadc3);
		}
/*check how many “emergency citation” we hew*/
		RedLedBlink = 0;
		for (uint8_t i = 0; i < 3; i++) {

			if (start_adc_val[i] > end_adc_val[i])
			{
				if (adcValue[i] <= trig_val[i] + histereis[i])
				{
					RedLedBlink++;
				}
			} else if (adcValue[i] >= trig_val[i] + histereis[i]) {
				RedLedBlink++;
			}
		}
/*convert ADC value to percent and set PWM duty cycle */
		uint8_t prst0 = ADC_to_percent(start_adc_val[0], end_adc_val[0],
				adcValue[0]);
		_Tim_set_timer(orange_led, prst0, 1000);
		uint8_t prst1 = ADC_to_percent(start_adc_val[1], end_adc_val[1],
				adcValue[1]);
		_Tim_set_timer(green_led, prst1, 1000);
		uint8_t prst2 = ADC_to_percent(start_adc_val[2], end_adc_val[2],
				adcValue[2]);
		_Tim_set_timer(blue_led, prst2, 1000);

	/*check if  “emergency citation” 	*/
		if (RedLedBlink != 0) { /*if yes setup blink time and blink*/
			if (HAL_GetTick() - lastTick >= maxTick) {

				switch (RedLedBlink) {
				case (1):
					maxTick = temp_high_mS1;
					break;
				case (2):
					maxTick = temp_high_mS2;
					break;
				case (3):
					maxTick = temp_high_mS3;
					break;

				default:
					Error_Handler();
				}
				lastTick = HAL_GetTick();
				HAL_GPIO_TogglePin(GPIOD, red_led);
			}
		} else {
			HAL_GPIO_WritePin(GPIOD, red_led, GPIO_PIN_RESET);
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
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
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
