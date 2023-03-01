#pragma once
#include <stdint.h>
#include "main.h"

void _Tim_stop(void);//stop all timers
void _Tim_init(void);//first init timer and set default settings
void _Tim_set_timer(uint32_t channel, uint32_t pulse, uint32_t freqency);
