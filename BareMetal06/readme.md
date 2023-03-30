#  PCA9685 I2C-bus LED controller library for STM32 
# 16-channel, 12-bit PWM Fm+
## Tasks 
This library created to do this task [Bare Metal Training 06:I2C](https://docs.google.com/document/d/1Tv1X7HTzzbWKn_u_Pb2Cj_DFPGNvzzRR/edit#heading=h.gjdgxs)
 ## Library consist
[pca9685.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Src/pca9685.c "pca9685.c")
[pca9685.h](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Inc/pca9685.h "pca9685.h")
  To work with this library just  include in main function.
  ``` C 
  #include pca9685.h 
  ``` 
  # Connecting PCA9685 to MCU  
![](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Screenshot%20from%202023-03-29%2019-42-00.png) <br>
# Settings
PWM controller PCA9685, has PIN A0-A4 they select slave address, more info [PIN 7.1.1 Regular I2C-bus slave address](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK)
in library we can change this address just write to this function. 
``` C 
_pwm_init(uint8_t dewID)
```
In [Embedded Starter kit](https://www.globallogic.com/ua/embedded-starter-kit/), default adress is 0x80, and can`t change it.
PWM controller has a external clock pin up to 50MHz (max) and internal clock sourse 25MHz, in [Embedded Starter kit](https://www.globallogic.com/ua/embedded-starter-kit/) external pin not used, we can use only internal clock generator. If need use external pin just define you new clock generator:
```C
#define OSC_CLOCK 25000000 // osc clock internal
```
And you can change range of 1% PCA9685 (12bit value 4096) is 40.96
```c
#define one_percent 40.96
```
# Addition function
In [main.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Src/main.c "main.c") i write  function **Process_Command(char*command)**  it is called when something come in to  UART buffer, and all symbol written in **cmd[]**. Then **Process_Command(char*command)** analyze and find existing symbols (name of function) in array,  for find i use *strcmp*, and find how many arguments in function i used *sscanf* it separated arguments by space symbol (%63s). Then check if arguments not out ob bound and print *OK* or *argument out of bounds/unknown argument/command unknown*

# Function
## void _pwm_set_freq(uint16_t freq) 
This function set PWM frequency (range 24-1524Hz) more info [7.3.5 PWM frequency PRE_SCALE](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK)
But when I test this controller I see frequency deviate when frequency set >1000Hz, for example if frequency set 1524 in output you have ~1600Hz ( I test it only with internal clock generator.) 
## uint8_t pwm_writefunc(uint8_t PIN, uint16_t HIGH, uint16_t LOW)
This function can setup turn on and turn off time off in PWM, PIN can be range 0-15, often used to write determined coefficients. Value of **HIGH** and **LOW** must bean in range 0-4095 if someone value be out of bounds, function return 1,2, or 3 if all correct returned 0. better used this function if you has already determined  coefficient and need write it. For [example](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK) from datasheet p17:
>Example 1: (assumes that the LED0 output is used and
(delay time) + (PWM duty cycle) > 100 %)
Delay time = 10 %; PWM duty cycle = 20 % (LED on time = 20 %; LED off time = 80 %).
Delay time = 10 % = 409.6 ~ 410 counts = 19Ah.
Since the counter starts at 0 and ends at 4095, we will subtract 1, so delay time = 199h
counts.
LED0_ON_H = 1h; LED0_ON_L = 99h (LED start turn on after this delay count to 409)
LED on time = 20 % = 819.2 ~ 819 counts.
LED off time = 4CCh (decimal 410 + 819 - 1 = 1228)
LED0_OFF_H = 4h; LED0_OFF_L = CCh (LED start turn off after this count to 1228)

So we have HIGH time 409 and LOW time 1228, write it:
```C
pwm_writefunc(1,409,1228); 
```
If we not want every time determine this coefficients, just use this function
## void _pwm_set_duty_delay(uint8_t PIN, uint8_t duty_cycle, uint8_t start_delay_percent) 
This simple usage function can select duty_cycle **duty cycle** in percent,  startup delay in percent **start_delay_percent**, and select pin **PIN** rage (0-15)
Function has two algorithm to determine coefficients:
*if start_delay_percent + dyty_cycle>100%, duty_cycle has range 0-99% and start_delay_percent 0-100%
if start_delay_percent + dyty_cycle<=100% duty_cycle has range 0-100% and start_delay_percent 0-100%*
more info [here](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK) on p16.
For example  pin 5, duty 30,start delay 0:
```C
_pwm_set_duty_delay(5,30,0);
```
## void _pwm_sleep(STATE status)
This function turn off oscillator and output pin of PWM controller, used it in setup frequency, reset, select clock source function, or power save mode.   (After use **wake** in this function, need manually setup frequency, else frequency be is 200Hz default.) Function catch one of this argument **sleep**, or **wake** 

## void _pwm_out_trigger(TRIGG status) 
Used to turn on/off all pwm pin or change polarity(OUTNE bits), catch one of this argument **OFF /ON** 
The active OFF output enable (OE) pin, allows to enable or disable all the LED outputs at
the same time.
• When a OFF level is applied to OE pin, all the LED outputs are enabled and follow
the output state defined in the LEDn_ON and LEDn_OFF registers with the polarity
defined by INVRT bit (MODE2 register).
• When a HIGH level is applied to OE pin, all the LED outputs are programmed to the
value that is defined by OUTNE[1:0] in the MODE2 register.
# How to run example
-   Compile project in CubeIde and load program to MCU,
-  Connect 12v or 5v power supply, to board and connect  LED to P3 connector and ground P5.
-   connect UART-USB connector,
-   open COM terminal [YAT](https://sourceforge.net/projects/y-a-terminal/) terminal for example, select your COM port, setup terminal baud rate 115200, 8bit, no parity, 1 stop bit.
-   open session and press a black button on Discovery board, if all done you see massage
>comand list:
>trigger <on/off>
>pulse <0-15> <0-100> (PIN Duty cycle)
>freqmod <24-1526>
>slepmod <off/on>

then you can write one of this command and show result
if function value done, you se massage 
>OK

else 
>unknown argument 

or
>command unknown

or 
>argument out of bounds

[![example test video]({https://drive.google.com/file/d/11ZoxVJghhQJIToPi-CTzMsDcrM4N0YYp/view?usp=share_link})]({https://drive.google.com/file/d/11ZoxVJghhQJIToPi-CTzMsDcrM4N0YYp/view?usp=share_link} "Link Title")
