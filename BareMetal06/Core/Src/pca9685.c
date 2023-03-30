/*Whiten by Mykhailo Svirskyi 05.03.2023*/
#include "gpio.h"
#include "main.h"
#include <string.h>
#include "i2c.h"
#include "pca9685.h"
#define OSC_CLOCK 25000000 // osc clock internal
#define one_percent 40.96
#define OE_PORT GPIOB
#define OE_PIN  GPIO_PIN_7
uint8_t dewId = 0x80; // default address
#define LED_ON(start_delay) (((start_delay) * (one_percent)) - 1)
#define LED_OFF(duty, led_on) (((duty) * (one_percent)) + (led_on)-1)
/*void _pwm_reset(void) //not work
{
    _pwm_sleep(sleep);
    HAL_Delay(1);
    uint8_t TxBuffer[8] = {0};
    TxBuffer[0] = 0x00; // address

    TxBuffer[1] = 0x80; // restart

    if ((HAL_I2C_Master_Receive(&hi2c1, dewId + 1, (uint8_t *)&TxBuffer, 2, 1000) >> 7) == 1)
    {
        TxBuffer[1] = 0x01; // clear bit 4
        HAL_Delay(100);
        HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
    }
    HAL_Delay(100);
    TxBuffer[1] = 0x80;
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
    TxBuffer[1] = 0x80;
    TxBuffer[0] = 0x06; // address
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
}*/

void _pwm_set_duty_delay(uint8_t PIN, uint8_t dyty_cycle,
                         uint8_t start_delay_percent) // if start_delay_percent + dyty_cycle>100%, dyty_cycle has range 0-99% and start_delay_percent 0-100%
                                                      // if start_delay_percent + dyty_cycle<=100% dyty_cycle has range 0-100% and start_delay_percent 0-100%
{
    uint16_t on = 0;
    uint16_t off = 0;
    uint16_t off2 = 0;
    if ((dyty_cycle + start_delay_percent) <= 100) // if dyty_cycle+start_delay_percent<=100%
    {
        if (start_delay_percent != 0)
        {
            on = LED_ON(start_delay_percent);
            off = LED_OFF(dyty_cycle, on);
        }
        else
        {
            on = 0;
            off = LED_OFF(dyty_cycle, on);
        }
    }
    else
    { // if dyty_cycle+start_delay_percent)>100%
        /*			on=0;
         off=LED_OFF(dyty_cycle,0);*/
        on = LED_ON(start_delay_percent);
        off2 = LED_ON(dyty_cycle);
        off = (on + off2) - 4095;
    }
    pwm_writefunc(PIN, on, off);
};
void _pwm_set_freq(uint16_t freq)
{
    if (freq <= 1526 && freq >= 24)
    {
        _pwm_sleep(sleep);
        uint16_t prescaler;
        prescaler = (OSC_CLOCK / (4096 * freq)) - 1;
        uint8_t TxBuffer[2] = {0};
        TxBuffer[0] = 0xFE; // PRE_SCALE
        TxBuffer[1] = (prescaler > 0x03) ? prescaler : 0x03;
        HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
        _pwm_sleep(wake);
    }
}

uint8_t pwm_writefunc(uint8_t PIN, uint16_t HIGH, uint16_t LOW)
{
    uint8_t start_ledaddr = 0x06; // led0 ON LOW byte address + 1 ON HIGH byte address, and if start_ledaddr+2 its LOW byte address

    if (PIN < 0 && PIN > 15)
    {
        return (1);
    }
    if (HIGH > 0xFFF)
    {
        return (2);
    }
    if (LOW > 0xFFF)
    {
        return (3);
    }
    uint8_t TxBuffer[8] = {0};

    start_ledaddr = start_ledaddr + 4 * PIN; // add to address +4 every next pin

    TxBuffer[0] = start_ledaddr; // LED_ON_L
    TxBuffer[1] = HIGH & 0xff;   // write LOW byte
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);

    TxBuffer[0] = start_ledaddr + 1; // LED_ON_H
    TxBuffer[1] = HIGH >> 8;         // werite High byte
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);

    TxBuffer[0] = start_ledaddr + 2; // LED_ON_L
    TxBuffer[1] = LOW & 0xff;        // write LOW byte
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);

    TxBuffer[0] = start_ledaddr + 3; // LED_ON_H
    TxBuffer[1] = LOW >> 8;          // werite High byte
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);

    return (0);
}
void _pwm_init(uint8_t dewID)
{
    dewId = dewID;
    uint8_t TxBuffer[8] = {0};
    TxBuffer[0] = 0x00; // MODE0
    TxBuffer[1] = 0x01;
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
};
void _pwm_sleep(STATE status)
{
    uint8_t TxBuffer[8] = {0};
    TxBuffer[0] = 0x00; // address
    if (status == sleep)
    {
        TxBuffer[1] = 0x11; // sleep on
    }
    else
    {
        TxBuffer[1] = 0x01;
    }
    HAL_I2C_Master_Transmit(&hi2c1, dewId, (uint8_t *)&TxBuffer, 2, 1000);
};
void _pwm_out_trigger(TRIGG status) // turn OFF/ON all pwm pin
{
    if (status == OFF)
    {
        HAL_GPIO_WritePin(OE_PORT, OE_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GOE_PORT, OE_PIN, GPIO_PIN_RESET);
    }
};
