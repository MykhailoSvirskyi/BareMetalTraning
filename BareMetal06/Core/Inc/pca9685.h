typedef enum
{
  sleep = 0,
  wake
}STATE;

typedef enum
{
  OFF = 0,
  ON
}TRIGG;

void _pwm_set_duty_delay(uint8_t PIN, uint8_t dyty_cycle,uint8_t start_delay_percent);
void _pwm_set_freq(uint16_t freq);
void _pwm_sleep( STATE status);
void _pwm_reset(void);
void _pwm_out_trigger(TRIGG state);
uint8_t pwm_writefunc(uint8_t PIN, uint16_t HIGH, uint16_t LOW);
