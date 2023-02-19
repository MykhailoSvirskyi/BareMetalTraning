/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED4_Pin GPIO_PIN_12
#define LED4_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_13
#define LED3_GPIO_Port GPIOD
#define LED5_Pin GPIO_PIN_14
#define LED5_GPIO_Port GPIOD
#define LED6_Pin GPIO_PIN_15
#define LED6_GPIO_Port GPIOD
#define sw4_speed_down_Pin GPIO_PIN_6
#define sw4_speed_down_GPIO_Port GPIOC
#define sw4_speed_down_EXTI_IRQn EXTI9_5_IRQn
#define sw5_speed_up_Pin GPIO_PIN_8
#define sw5_speed_up_GPIO_Port GPIOC
#define sw5_speed_up_EXTI_IRQn EXTI9_5_IRQn
#define sw3_back_Pin GPIO_PIN_9
#define sw3_back_GPIO_Port GPIOC
#define sw3_back_EXTI_IRQn EXTI9_5_IRQn
#define sw2_on_off_Pin GPIO_PIN_15
#define sw2_on_off_GPIO_Port GPIOA
#define sw2_on_off_EXTI_IRQn EXTI15_10_IRQn
#define sw1_next_Pin GPIO_PIN_11
#define sw1_next_GPIO_Port GPIOC
#define sw1_next_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
