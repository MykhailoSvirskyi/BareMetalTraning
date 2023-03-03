#include "main.h"
#include "usart.h"
#include <string.h>

void
_LED_STATUS (GPIO_TypeDef *gPIOx, uint16_t gPIO_Pin)
{

  char gpioBLUE[] = "BLUE LED";
  char gpioRED[] = "RED LED";
  char gpioGREEN[] = "GREEN LED";
  char gpioORANGE[] = "ORANGE LED";
  char gpionotselect[] = "unknown gPIOx";
  uint32_t pointer;
  uint8_t size;
  char off[] = " OFF";
  char on[] = "  ON";

  HAL_GPIO_TogglePin (gPIOx, gPIO_Pin); //set pin

  if (gPIOx == GPIOD)
    {
      switch (gPIO_Pin)
	{
	case LED_GREEN_Pin:
	  pointer = gpioGREEN;
	  size = strlen ((char*) gpioGREEN);
	  break;

	case LED_ORANGE_Pin:
	  pointer = gpioORANGE;
	  size = strlen ((char*) gpioORANGE);
	  break;

	case LED_RED_Pin:
	  pointer = gpioRED;
	  size = strlen ((char*) gpioRED);
	  break;

	case LED_BLUE_Pin:
	  pointer = gpioBLUE;
	  size = strlen ((char*) gpioBLUE);
	  break;
	default:
	  pointer = gpionotselect;
	  size = strlen ((char*) gpionotselect);
	  break;
	}
    }
  else
    {
      HAL_UART_Transmit (&huart3, "PORT not D\r\n", 10, 100);
    }
  HAL_UART_Transmit (&huart3, pointer, size, 100); //send name of pin

  if (HAL_GPIO_ReadPin (gPIOx, gPIO_Pin) == GPIO_PIN_SET) //check if on/off and print in terminal //not work if open-drain mode!
    {
      HAL_UART_Transmit (&huart3, on, strlen ((char*) on), 100);
    }
  else
    {
      HAL_UART_Transmit (&huart3, off, strlen ((char*) off), 100);
    }
  HAL_UART_Transmit (&huart3, "\r\n", 2, 100);
}

void
_Button_reader (uint16_t GPIO)
{

  switch (GPIO)
    {
    case SWT1_Pin:
      _LED_STATUS (GPIOD, LED_BLUE_Pin);
      break;

    case SWT2_Pin:
      _LED_STATUS (GPIOD, LED_BLUE_Pin);
      _LED_STATUS (GPIOD, LED_ORANGE_Pin);
      _LED_STATUS (GPIOD, LED_RED_Pin);
      _LED_STATUS (GPIOD, LED_GREEN_Pin);

      break;

    case SWT3_Pin:
      _LED_STATUS (GPIOD, LED_ORANGE_Pin);
      break;

    case SWT4_Pin:
      _LED_STATUS (GPIOD, LED_RED_Pin);
      break;

    case SWT5_Pin:
      _LED_STATUS (GPIOD, LED_GREEN_Pin);
      break;
    }
}
;

