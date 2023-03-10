/* USER CODE BEGIN Header */
/*Author Mykhailo Svirskyi 05.03.2023*/
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pca9685.h"
void Process_Command(char *command);
#define buffsize 128    // uart3 input buffer size
#define CMD_SIZE (128)  // process_command bufer
#define BUF_SIZE (1024) // uart write buffer size
char cmd[CMD_SIZE];
uint8_t tx_buf[BUF_SIZE];
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
    MX_I2C1_Init();
    MX_USART3_UART_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    HAL_Delay(200);
    snprintf((char *)tx_buf, BUF_SIZE, "comand list:\r\ntrigger <on/off>\r\npulse <0-15> <0-100> (PIN Duty cycle)\r\nfreqmod <24-1526>\r\nslepmod <off/on>\r\n");
    HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);
/*    _pwm_reset();*/
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
    char args[64];
    char args2[64];
    uint32_t argi, argi2;
    uint8_t parsed = sscanf(command, "%63s %63s %63s", cmd, args, args2);
    if (parsed == 3)
    {
        if (strcmp(cmd, "pulse") == 0)
        {
            snprintf((char *)tx_buf, BUF_SIZE, "OK\r\n");
            if (isdigit(args[0]))
            {
                argi = strtol(args, (char *)NULL, 10);
                if (argi >= 0 && argi <= 15)
                {
                    if (isdigit(args2[0]))
                    {
                        argi2 = strtol(args2, (char *)NULL, 10);
                        if (argi2 >= 0 && argi <= 100)
                        {

                            _pwm_set_duty_delay(argi, argi2, 0);
                            snprintf((char *)tx_buf, BUF_SIZE, "OK\r\n");
                        }
                        else
                        {
                            snprintf((char *)tx_buf, BUF_SIZE, "argument out of bounds <0 or >100%\r\n");
                        }
                    }
                    else
                    {
                        snprintf((char *)tx_buf, BUF_SIZE, "argument out of bounds <0 or >15\r\n");
                    }
                }
                else
                {
                    snprintf((char *)tx_buf, BUF_SIZE, "unknown argument\r\n");
                }
            }
        }
        else
        {
            snprintf((char *)tx_buf, BUF_SIZE, "command unknown\r\n");
        }
    }

    else if (parsed == 2 || parsed == 1)
    {
        if (strcmp(cmd, "trigger") == 0)
        {
            if (strcmp(args, "on") == 0)
            {
                snprintf((char *)tx_buf, BUF_SIZE, "trigger on\r\n");
                _pwm_out_trigger(ON);
            }
            else if (strcmp(args, "off") == 0)
            {
                snprintf((char *)tx_buf, BUF_SIZE, "trigger off\r\n");
                _pwm_out_trigger(OFF);
            }
            else
            {
                snprintf((char *)tx_buf, BUF_SIZE, "unknown trigger argument\r\n");
            }
        }

        else if (strcmp(cmd, "slepmod") == 0)
        {
            if (strcmp(args, "off") == 0)
            {
                snprintf((char *)tx_buf, BUF_SIZE, "wake\r\n");
                _pwm_sleep(wake);
            }
            else if (strcmp(args, "on") == 0)
            {
                snprintf((char *)tx_buf, BUF_SIZE, "sleep\r\n");
                _pwm_sleep(sleep);
            }
            else
            {
                snprintf((char *)tx_buf, BUF_SIZE, "unknown sleepmode argument\r\n");
            }
        }

        else if (strcmp(cmd, "freqmod"
                             "") == 0)
        {
            if (isdigit(args[0]))
            {
                argi = strtol(args, (char *)NULL, 10);
                if (argi >= 24 && argi <= 1526)
                {
                    _pwm_set_freq(argi);
                    /*					set freqency*/
                    snprintf((char *)tx_buf, BUF_SIZE, "OK\r\n");
                }
                else
                {
                    snprintf((char *)tx_buf, BUF_SIZE, "argument out of bounds <24 or >1526\r\n");
                }
            }
            else
            {
                snprintf((char *)tx_buf, BUF_SIZE, "freqmod unknown argument\r\n");
            }
        }

        else
        {
            snprintf((char *)tx_buf, BUF_SIZE, "command unknown2\r\n");
        }
    }

    else if (parsed == 1 && strcmp(cmd, "clear") == 0)
    {

        snprintf((char *)tx_buf, BUF_SIZE, "OK\r\n");
    }
    else
    {
        snprintf((char *)tx_buf, BUF_SIZE, "command unknown\r\n");
    }
    HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);
    // LCD_Printf("<< %s", tx_buf);
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
