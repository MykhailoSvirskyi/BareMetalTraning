#pragma once
#include <stdint.h>
#include "main.h"

typedef struct
{
    uint32_t channel;
    uint32_t pulse;
    uint32_t freqency;

} PWM;


void _Tim_stop(void);//stop all timers
void _Tim_imit(void);//first init timer and set default settings
void _Tim_second_init(void);// change settings
void _Button_reader(uint16_t GPIO_Pin);//read pin press
