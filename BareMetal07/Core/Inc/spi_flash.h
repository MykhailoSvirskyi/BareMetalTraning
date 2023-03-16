#pragma once

typedef enum
{
    none = 0,
    first1of32 = 1,
    first1of16 = 2,
    first1of8 = 3,
    first1of4 = 4,
    first1of2 = 5,
    all = 6
} block_protection; // BLOCK PROTECTION (BP3,BP2,BP1, BP0);
typedef enum
{
    BUSY = 0, // 1 = busy; 0 = not busy;
    WEL,      // WEL  if ==1 can write memory, if ==0 can`t write.
    BP0,      // Indicate current level of block write protection
    BP1,
    BP2,
    BP3, // NOT used
    AAI, // Auto Address Increment Programming status
         // 1 = AAI programming mode 0 = Byte-Program mode
    BPL // BPL 0 = BP are read/writable; 1=BP are are read-only bits
} flash_status;
uint8_t flash_init(SPI_HandleTypeDef *hspi);
void block_register(block_protection qq);
uint8_t flash_read_status(flash_status ww);
void flash_write_enable(void);
void flash_write_disable(void);
void flash_byte_write(uint8_t byte, uint32_t address);
void flash_read(uint32_t address, uint8_t *receive);
void flash_erase(void);
uint8_t reverseBits(uint8_t v);
