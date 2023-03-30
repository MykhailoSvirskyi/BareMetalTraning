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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "button.h"
#define ADC_RANGE 4096
#define ADC_VOLT 3.3
#define ADC_STEP (ADC_RANGE/ADC_VOLT)
#define ADC_100C (0.02*ADC_STEP)
#define ADC_0C (2.02*ADC_STEP)
#define ADC_minus_24C (2.52*ADC_STEP)
#define ADC_100C_0C_coef ((ADC_0C)/100)
#define temp_corect 10 //corection temperature
#define ADC_TO_CELSIUS(adc) ((adc)<(ADC_0C))? (100+(temp_corect)-((adc)/(ADC_100C_0C_coef))):(-88)//if ADC<2.02v t=0C, and  adc=0.02v t=99C
//If the return -88 range ADC dropped below 0C
#define temp_time_every_ms 5000 //send temperature every 5s;
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
#define DEBOUNCE 100 // button debounce timing
uint32_t last_time = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config (void);
/* USER CODE BEGIN PFP */
void
HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
  uint32_t time = HAL_GetTick ();

  if (time - last_time > DEBOUNCE)
    {
      _Button_reader (GPIO_Pin);
    }
  last_time = time;

}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main (void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init ();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config ();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init ();
  MX_USART3_UART_Init ();
  MX_ADC2_Init ();
  /* USER CODE BEGIN 2 */
  volatile HAL_StatusTypeDef adcPoolResult;
  uint32_t lastTick = 0, maxTick = temp_time_every_ms,      adcValue = 0;
  int8_t temp = 0;
  uint8_t tx[64];
  uint8_t rx;
  HAL_UART_Transmit (&huart3, "togle led pin use $<> 1-4 if all led $5\r\n", 41,100); /*start text*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {
/*temperature deterimine and send */
      if (HAL_GetTick () - lastTick >= maxTick)
	{

	  HAL_ADC_Start (&hadc2);
	  adcPoolResult = HAL_ADC_PollForConversion (&hadc2, 100);
	  if (adcPoolResult == HAL_OK)
	    {
	      adcValue = HAL_ADC_GetValue (&hadc2);
	    }
	  lastTick = HAL_GetTick ();
	  temp = ADC_TO_CELSIUS(adcValue);
	  sprintf (tx, "temperature=%d C\r\n", temp); //
	  HAL_UART_Transmit (&huart3, tx, strlen ((char*) tx), 100);

	}
/*read command from terminal, setup pin*/
      if (HAL_UART_Receive (&huart3, &rx, 1, 10) == HAL_OK)
	{
	  if (rx == '\r' || rx == '\n')
	    {
	    } /* If \r or \n print text */

	  else if (rx == '$'&& HAL_UART_Receive (&huart3, &rx, 1, 10) == HAL_OK) //read second byte from UART buffer
	    {
	      switch (rx)
		{
		case '1':
		  _LED_STATUS (GPIOD, LED_BLUE_Pin); //Toggle pin and print to terminal pin status
		  break;
		case '2':
		  _LED_STATUS (GPIOD, LED_ORANGE_Pin);
		  break;
		case '3':
		  _LED_STATUS (GPIOD, LED_RED_Pin);
		  break;
		case '4':
		  _LED_STATUS (GPIOD, LED_GREEN_Pin);
		  break;
		case '5':
		  _LED_STATUS (GPIOD, LED_BLUE_Pin);
		  _LED_STATUS (GPIOD, LED_ORANGE_Pin);
		  _LED_STATUS (GPIOD, LED_RED_Pin);
		  _LED_STATUS (GPIOD, LED_GREEN_Pin);
		  break;

		default:
		  HAL_UART_Transmit (&huart3, "unknown command\r\n", 2, 100);
		  HAL_UART_Transmit (
		      &huart3, "togle led pin use $<> 1-4 if all led $5\r\n", 41, 100);
		  break;
		}
	    }
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
void
SystemClock_Config (void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct =
    { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct =
    { 0 };

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
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler ();
    }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
      Error_Handler ();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler (void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq ();
  while (1)
    {
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
