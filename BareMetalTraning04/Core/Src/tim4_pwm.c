#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "tim4_pwm.h"
#define TIM_QANTY 3

uint16_t cHANNEL[TIM_QANTY] = {

TIM_CHANNEL_1,
TIM_CHANNEL_2,
/* TIM_CHANNEL_3,*/
TIM_CHANNEL_4 };

uint32_t channel[TIM_QANTY]; //

const uint32_t FREQMAX = 100;
const uint32_t SYSFREQ = 20000;

uint8_t curr_channel = 0;

void _Tim_stop(void) {
	for (uint16_t i = 0; i < TIM_QANTY; i++) {
		HAL_TIM_PWM_Stop(&htim4, cHANNEL[i]);
	}
}
;

void _Tim_set_timer(uint32_t channel, uint32_t pulse, uint32_t freqency) {

	if (pulse != 0) {
		uint32_t ccr = 0;

		if (freqency > 0 && freqency < (10 * SYSFREQ) && pulse <= 100) //determine ARR and CCR TIM4;
				{
			TIM4->ARR = (uint32_t) ((SYSFREQ / (freqency)) - 1);
			ccr = (uint32_t) (((pulse) * (TIM4->ARR)) / 100);
		} else {
			HAL_TIM_PWM_Stop(&htim4, channel);
		}

		switch (channel) {
		case TIM_CHANNEL_1:
			TIM4->CCR1 = ccr;
			break;
		case TIM_CHANNEL_2:
			TIM4->CCR2 = ccr;
			break;
		case TIM_CHANNEL_3:
			TIM4->CCR3 = ccr;
			break;
		case TIM_CHANNEL_4:
			TIM4->CCR4 = ccr;
			break;
		case TIM_CHANNEL_ALL:
			TIM4->CCR1 = ccr;
			TIM4->CCR2 = ccr;
			TIM4->CCR3 = ccr;
			TIM4->CCR4 = ccr;
			break;
		default:
			TIM1->CCR1 = ccr;
			break;
		}

		HAL_TIM_PWM_Start(&htim4, channel);
	} else {
		HAL_TIM_PWM_Stop(&htim4, channel);
	}

}
;
void _Tim_init(void) { // default start setings

	HAL_TIM_Base_Init(&htim4);
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_PWM_Init(&htim4);

}
;

