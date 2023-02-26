#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "TIM4_PWM.h"
#define tim_pin_quantity 4
const uint32_t FREQMAX = 100;
const uint32_t SYSFREQ = 80000;
const uint32_t FREQSTEP = 5;
const uint32_t FREQMIN = FREQSTEP;
const uint32_t PULSESTEP = 5;

PWM timset;

uint16_t cHANNEL[tim_pin_quantity + 1] = {
                                          TIM_CHANNEL_1,
                                          TIM_CHANNEL_2,
                                          TIM_CHANNEL_3,
                                          TIM_CHANNEL_4,
                                          TIM_CHANNEL_ALL};
uint8_t curr_channel = 0;

void _Button_reader(uint16_t GPIO_Pin)
{

    switch (GPIO_Pin)
    {
    case SWT1_Pin: // set signal frequency ( down)
        if (timset.freqency >= FREQMIN)
        {
            timset.freqency -= FREQSTEP;
        }
        break;

    case SWT2_Pin: // select signal output (PD15, PD14, PD13, PD12 or no output (disable case)). Cyclic selection scheme
        if (curr_channel < tim_pin_quantity)
        {
            curr_channel++;
            timset.channel = cHANNEL[curr_channel];
        }
        else
        {
            curr_channel = 0;
            timset.channel = cHANNEL[curr_channel];
        }
        if (curr_channel == tim_pin_quantity) /*last position disable all pin*/
        {
            _Tim_stop();
        }

        break;

    case SWT3_Pin: // set signal frequency (up)
        if (timset.freqency < FREQMAX)
        {
            timset.freqency += FREQSTEP;
        }
        break;

    case SWT4_Pin: //  duty cycle (+ PULSESTEP% step each button press)
        if (timset.pulse < (100 - PULSESTEP))
        {
            timset.pulse += PULSESTEP;
        }
        break;

    case SWT5_Pin: //  duty cycle (- PULSESTEP% step each button press)
        if (timset.pulse > (PULSESTEP + PULSESTEP))
        {
            timset.pulse -= PULSESTEP;
        }
        break;
    }
}

void _Tim_stop(void)
{

    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
}
void _Tim_imit(void)
{ // default start setings

    timset.channel = TIM_CHANNEL_1;
    timset.freqency = FREQMIN;
    timset.pulse = PULSESTEP;
    HAL_TIM_Base_Init(&htim4);
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_PWM_Init(&htim4);
    TIM4->ARR = (uint32_t)((SYSFREQ / timset.freqency) - 1);
    TIM4->CCR1 = (uint32_t)(((timset.pulse) * ((TIM4->ARR) + 1)) / 100);
    HAL_TIM_PWM_Start(&htim4, timset.channel);

    // uint16_t ARRvalue=(uint16_t)((SYSFREQ/timset.freqency)-1);
    // uint32_t CCRvalue=(uint16_t)(((timset.pulse)*(TIM4.ARR+1))/100);
};
void _Tim_second_init(void)
{
    _Tim_stop();
    uint32_t ccr = 0;

    if (timset.freqency > 0)//determine ARR and CCR TIM4;
    {
        TIM4->ARR = (uint32_t)((SYSFREQ / (timset.freqency)) - 1);
        ccr = (uint32_t)(((timset.pulse) * (TIM4->ARR)) / 100);
    }
    else
    {
        _Tim_stop();
    }

    switch (timset.channel)
    {
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
        TIM4->CCR1 = 0;
        TIM4->CCR2 = 0;
        TIM4->CCR3 = 0;
        TIM4->CCR4 = 0;
        break;
    default:
        TIM1->CCR1 = ccr;
        break;
    }
    if (timset.channel != TIM_CHANNEL_ALL)
    {
        HAL_TIM_PWM_Start(&htim4, timset.channel);
    }
};
