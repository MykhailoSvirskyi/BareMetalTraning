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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define BUF_SIZE (64) // uart write buffer size
#include "spi_flash.h"

void Process_Command(char *command);
#define buffsize 128   // uart3 input buffer size
#define CMD_SIZE (128) // process_command bufer

char cmd[CMD_SIZE] = {0};
uint8_t tx_buf[BUF_SIZE] = {0};
uint8_t icmd = 0;
uint8_t rx;
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

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART3_UART_Init();
    /* USER CODE BEGIN 2 */
    if (flash_init(&hspi1) != 1)
    {
    	snprintf((char *)tx_buf, BUF_SIZE, "init spi flash ERROR:\r\n");
    	    HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);
        Error_Handler();
    }

    block_register(none);


    snprintf((char *)tx_buf, BUF_SIZE, "init spi flash OK:\r\n"
                                       "comand:\r\n"
                                       "read, erase, write");
    HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        if (HAL_UART_Receive(&huart3, &rx, 1, 10) == HAL_OK)
        {

            do
            {
                cmd[icmd++] = rx;
            } while (HAL_UART_Receive(&huart3, &rx, 1, 10) == HAL_OK && rx != '\n');
            icmd = 0;
            Process_Command(cmd);
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
    RCC_OscInitStruct.PLL.PLLN = 64;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */


void Process_Command(char *command)
{

	char cmd[64];

	uint8_t parsed = sscanf(command, "%63s", cmd);

    if (parsed == 2 || parsed == 1)
    {
        if (strcmp(cmd, "read") == 0)
        {
            for (uint32_t ptr = 0; ptr <= 0x1FFFF;)
            {
                flash_read(ptr, tx_buf);
                HAL_UART_Transmit(&huart3, tx_buf, 5, 100);

/*                if ((ptr & 0x1F) == 0x1F)			//manualy print \r\n
                {
                    HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", 2, 100);
                }*/
                ptr = ptr + 5;
            }
            snprintf((char *)tx_buf, BUF_SIZE, "read finish\r\n");
        }

        else if (strcmp(cmd, "erase") == 0)
        {
            flash_erase();
            snprintf((char *)tx_buf, BUF_SIZE, "erase OK\r\n");
        }

        else if (strcmp(cmd, "write") == 0)
        {
		#define BUF_S 2000
            uint8_t Tx_buf[BUF_S] = {0};
            snprintf((char *)Tx_buf, BUF_S,
                     "TIME CAPSULE\n"
                     "From: Mykhailo Svyrskyi Telegram: @soldermen\n"
                     "Mentor Artem Dovhal\n"
                     "Date: 16.03.2023\n"
                     "Wer zu Lebzeit gut auf Erden\n"
                     "wird nach dem Tod ein Engel werden\n"
                     "den Blick gen Himmel fragst du dann\n"
                     "warum man sie nicht sehen kann\n"
                     "Erst wenn die Wolken schlafengehn\n"
                     "kann man uns am Himmel sehn\n"
                     "wir haben Angst und sind allein\n"
                     "Gott weiss ich will kein Engel sein\n"
                     "Sie leben hinterm Sonnenschein\n"
                     "getrennt von uns unendlich weit\n"
                     "sie mussen sich an Sterne krallen (ganz fest)\n"
                     "damit sie nicht vom Himmel fallen");
            Tx_buf, strlen((char *)Tx_buf);

            for (uint32_t ptr = 0, ptr1 = 0, flcount = 0; ptr < strlen((char *)Tx_buf); ptr++, ptr1++)
            {

                flash_byte_write((uint8_t *)Tx_buf[ptr], ptr1 + flcount);
                // HAL_Delay(1);
                if (Tx_buf[ptr] == '\n')
                {
                    ptr1 = 0;
                    flcount = flcount + 4096;
                }
            }
            snprintf((char *)tx_buf, BUF_SIZE, "write finish\r\n");
        }
        else if (strcmp(cmd, "block") == 0){
        block_register(all);
        snprintf((char *)tx_buf, BUF_SIZE, "memory locked\r\n");
        }
        else
        {
            snprintf((char *)tx_buf, BUF_SIZE, "command unknown\r\n");
        }
    }

    HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);

}
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
