/* written by Mykhailo Svirskyi 10.03.2023*/
#include <stdio.h>
#include "spi.h"
#include "gpio.h"
#include "spi_flash.h"

uint32_t HSPI;
#define CS_port GPIOD
#define CS_pin GPIO_PIN_7
#define dev_id 0x41
uint8_t TransmitArray[20];
uint8_t ReceiveArray[20];

enum spi_flash_status
{
    FLASH_ERROR = 0,
    FLASH_OK
};
enum spi_flash_busy
{
    FLASH_READY = 0,
    FLASH_BUSY
};

uint8_t flash_init(SPI_HandleTypeDef *hspi)
{
    HSPI = hspi;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    TransmitArray[0] = 0x90;
    TransmitArray[1] = 0x00;
    TransmitArray[2] = 0x00;
    TransmitArray[3] = 0x01;
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(HSPI, TransmitArray, ReceiveArray, 8, 100);
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
    if (ReceiveArray[4] == dev_id || ReceiveArray[5] == dev_id)
    {
        return FLASH_OK;
    }
    else
    {
        return FLASH_ERROR;
    }
}

void flash_write_disable(void)
{
    uint8_t WRDI = 0x04; // WRITE DISABLE
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI, &WRDI, 1, 100); //
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
}

void flash_write_enable(void)
{
    uint8_t WREN = 0x06; // WRITE ENABLE
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI, &WREN, 1, 100); //
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
}

void block_register(block_protection qq) // if WR# pin not pull up need to call the program 2 times to set 7 bit BPL to 0
{
    flash_write_enable();
    uint8_t block_prot_byte[2] = {0}; // SOFTWARE STATUS REGISTER first byte adress
    block_prot_byte[0] = 0x01;        // first 8 bit its addres 01

    switch (qq)
    {
    case 0: // no block
        break;
    case 1:                        // 1/32
        block_prot_byte[1] = 0x04; // byte bp0
        break;
    case 2:                        // 1/16
        block_prot_byte[1] = 0x08; // byte bp1
        break;
    case 3:                        // 1/8
        block_prot_byte[1] = 0x0C; // byte bp1,byte bp0
        break;

    case 4:                        // 1/4
        block_prot_byte[1] = 0x10; // byte bp2
        break;

    case 5:                        // 1/2
        block_prot_byte[1] = 0x14; // byte bp2,	byte bp0
        break;

    case 6:                        // all
        block_prot_byte[1] = 0x1C; // byte bp2,	byte bp1,	byte bp0
        break;

    default:
        Error_Handler();
        break;
    }
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI, block_prot_byte, 2, 100);
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
}
uint8_t flash_read_status(flash_status ww)
{
    uint8_t receiveArray[2] = {0};
    uint8_t transmitArray[2] = {0};
    transmitArray[0] = 0x05;
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(HSPI, transmitArray, receiveArray, 2, 100); // READ-STATUS-REGISTER (RDSR) SEQUENCE
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
    uint8_t mask = 0x01;
    mask = mask << ww;
    return ((receiveArray[1] & mask) ? 1 : 0);
}

void flash_byte_write(uint8_t byte, uint32_t address)
{
try_again:
    if (flash_read_status(BUSY) == FLASH_READY && (address >> 21) == 0) // check if address not over 0x1fffff
    {
        // block_register(none);
        flash_write_enable();

        uint8_t transm[5] = {0};
        transm[0] = 0x02;
        transm[1] = (address & 0xff0000) >> 16;
        transm[2] = (address & 0xff00) >> 8;
        transm[3] = address & 0xff;
        transm[4] = byte;
        HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(HSPI, transm, 5, 100); //
        HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
    }
    else if ((address >> 21) == 0)
    {
        HAL_Delay(5);
        goto try_again;
    }
}

void flash_read(uint32_t address, uint8_t *receive)
{
    if (flash_read_status(BUSY) == 0 && (address >> 21) == 0) // check if address not over 0x1fffff
    {

        uint8_t transm[5] = {0};
        transm[0] = 0x0B;
        transm[1] = (address & 0xff0000) >> 16;
        transm[2] = (address & 0xff00) >> 8;
        transm[3] = address & 0xff;
        transm[4] = 0;

        HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);

        HAL_SPI_Transmit(HSPI, transm, 5, 100);
        HAL_SPI_Receive(HSPI, (uint8_t *)receive, 5, 100);

        // HAL_SPI_TransmitReceive(HSPI, &read,(uint8_t *)receive, 10, 100); //

        HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
    }
}
void flash_erase(void)
{
    block_register(none);
    flash_write_enable();
    uint8_t erase = 0x60; // Erase Full Memory Array
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(HSPI, &erase, 1, 100); //
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
}
