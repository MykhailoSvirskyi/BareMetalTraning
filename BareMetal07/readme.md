# SPI SST25VF016  Flash library for STM32 
## Tasks 
This library created to do this task [Bare Metal Training 07: SPI Flash](https://docs.google.com/document/d/1Tv1X7HTzzbWKn_u_Pb2Cj_DFPGNvzzRR/edit#heading=h.gjdgxs)
 ## Library consist
  [spi_flash.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Core/Src/spi_flash.c "spi_flash.c")
  [spi_flash.h](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Core/Inc/spi_flash.h "spi_flash.h")
  To work with this library, just include it in the main function.
  ``` C 
  #include spi_flash.h 
  ``` 

  # Connecting Flash to MCU  
![](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Screenshot%20from%202023-03-29%2011-21-34.png) <br>
In this schematic variant used manually Chip Select (CS) pin control, was because the auto CS pin is busy by another periphery. In   [spi_flash.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Core/Src/spi_flash.c "spi_flash.c")  you can select CS pin:  
``` C
#define CS_port GPIOD
#define CS_pin GPIO_PIN_7 
```
Pin must be set up as HIGH SPEED, and PULLUP (setup in CubeMX).
## Connecting UART 
I used USART3 with these settings, you can set up in CubeMX:
``` C
huart3.Instance = USART3;
huart3.Init.BaudRate = 115200;
huart3.Init.WordLength = UART_WORDLENGTH_8B;
huart3.Init.StopBits = UART_STOPBITS_1;
huart3.Init.Parity = UART_PARITY_NONE;
huart3.Init.Mode = UART_MODE_TX_RX;
huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
huart3.Init.OverSampling = UART_OVERSAMPLING_16;
```
And connect to RX ,TX, and GND  a UART-USB converter. in the laptop I use [YAT](https://sourceforge.net/projects/y-a-terminal/) terminal, select your COM port, other is default.
## Work with the library
First
  ``` C 
  #include spi_flash.h 
  ``` 
 Then write flash_init(&hspi) with your SPI port, for example:
 ``` C
 flash_init(&hspi1);
```
if this command is successful return 1, else 0;
``` C
FLASH_ERROR = 0,
FLASH_OK = 1
```
#### troubleshoot 
Some boards need external 5v power  ( you can take a jumper from the Discovery board 5V pin to P2 pin 5V) or connect 12v power supply. Sometimes need to add before starting Delay 100-300mS to take time to start FLASH IC.

This function read flash size for example, [read memory size (p17)](https://drive.google.com/file/d/1U1EzH6GznQjycxtpmeLms7kG1QVGYEOG/view?usp=share_link) if it`s SST25VF016 returned 0x41
you can change it in [spi_flash.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Core/Src/spi_flash.c "spi_flash.c") 
```C
#define dev_id 0x41
```
Recommend using this code before other functions, to look are all done
``` C
if (flash_init(&hspi1) != 1)
{
snprintf((char *)tx_buf, BUF_SIZE, "init spi flash ERROR:\r\n");
HAL_UART_Transmit(&huart3, tx_buf, strlen((char *)tx_buf), 100);
Error_Handler();
}
```
## Function 
####  uint8_t flash_read_status(flash_status ww)
This function is used to read the status register of FLASH IC. You can read one of these:
```C
typedef enum
{
BUSY = 0, // 1 = busy; 0 = not busy;
WEL, // WEL if ==1 can write memory, if ==0 can`t write.
BP0, // Indicate the current level of block write protection
BP1,
BP2,
BP3, // NOT used
AAI, // Auto Address Increment Programming status
// 1 = AAI programming mode 0 = Byte-Program mode
BPL // BPL 0 = BP are read/writable; 1=BP are are read-only bits
} flash_status;
```
Function return 0 or 1. This function can use every time, for example, are memory chip is still written/ read and it is busy or ready.
more info in datasheet [SST25VF016 ](https://drive.google.com/file/d/1U1EzH6GznQjycxtpmeLms7kG1QVGYEOG/view?usp=share_link)
#### void block_register(block_protection qq);
This function is used to block or unlock memory. You can select what memory block can be locked/unlocked (block started from address 0x000000; 
```C
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
```
![](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal07/Screenshot%20from%202023-03-29%2015-16-48.png) <br>
For example, need to unlock first of 1/2 memory 
```C
block_register(first1of2);
```
#### void flash_write_enable(void);
This function used to setup WRITE ENABLE LATCH (WEL) to 1. It needs to do every time before the write or erase function, after one of these commands FLASH IC automatically changed  WEL to 0.  **You can use this function if you create a new function that writes or erase FLASH, existed function consists of it.**
#### void flash_write_disable(void);
This function is used to setup WRITE ENABLE LATCH (WEL) to 0. 
#### void flash_byte_write(uint8_t byte, uint32_t address)
This function writes to FLASH a uint8_t byte in uint32_t address (address range for SST25VF016 is 0x000000-0x1FFFFF) if FLASH is busy **flash_byte_write()** wait at 5mS and try again, if used RTOS better change ```HAL_Delay(5);``` to you-re custom  delay function based on **HAL_GetTick** or some **Timer**, to not loosed command time, **warning if stay some error and FLASH send every time BUSY=1 function loped, better add functional who sent error message if send status BUSY=1 every N-times**
#### void flash_read(uint32_t address, uint8_t *receive);
This function is HIGH-SPEED-READ FLASH address from **address** and second 5 byte started from **address** and end of **address+4** 

#### void flash_erase(void)
 This function erases all FLASH. **Better manually check if BUSY is 0, if yes do this function.**

## Run example
+ Compile project in CubeIde and load program to MCU,
+ connect UART-USB connector, 
+ open COM terminal [YAT](https://sourceforge.net/projects/y-a-terminal/) terminal for example, select your COM port, setup terminal baud rate 115200, 8bit, no parity, 1 stop bit.
+ open session and press a black button on Discovery board, if all done you see massage <br> *init spi flash OK: <br>comand: <br> **read, erase, write, block***
if flash not response you see <br>*init spi flash ERROR:*
+ You can write in the terminal this command and have a result.
## TEST VIDEO
[bare metal test 07](https://drive.google.com/file/d/1RPCpbdCiq8guEHR2j3Ep79Pnv3PHwD1r/view?usp=share_link)
