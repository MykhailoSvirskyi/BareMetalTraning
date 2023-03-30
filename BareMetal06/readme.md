#  PCA9685 I2C-bus LED controller library for STM32 
# 16-channel, 12-bit PWM Fm+
## Tasks 
Need to create library that will work with PWM LED controller (PCA9685).
We need a way to enable/disable all outputs, set duty cycle, set PWM frequency, enable/disable sleep mode.
LED controller functionality should be moved to separate *.c file.
All public function prototypes (API) should be provided by *.h file.
I2C1 channel should be used to communicate with LED controller. SCK = 50 kHz (check frequency with signal analyzer see p. 5).
Functional testing should be made with help of PC (UART console) – send commands / receive status.
[OPTIONAL *]. Connect 16 LEDs to PWM outputs to control light intensity.
Need to capture signal trace from for outputs with help of signal analyzer to show that all functionality is working fine.
Trace should be saved as *.sal file and provided for mentor for analysis.
Project need to be stored with your own repository on the GitHub.
Provide it to your mentor for analysis and grade.
All other methods of transferring files are forbidden.
Need to capture video. Store it on a GoogleDrive (YouTube, etc…).
Provide a link to it for your mentor.
Enjoy!!! 😊
 ## Library consist
[pca9685.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Src/pca9685.c "pca9685.c")
[pca9685.h](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Inc/pca9685.h "pca9685.h")
  To work with this library just include it in the main function.
  ``` C 
  #include pca9685.h 
  ``` 
  # Connecting PCA9685 to MCU  
![](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Screenshot%20from%202023-03-29%2019-42-00.png) <br>
# Settings
PWM controller PCA9685, has PIN A0-A4 they select slave address, more info [PIN 7.1.1 Regular I2C-bus slave address](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK)
in a library, we can change this address by just writing to this function. 
``` C 
_pwm_init(uint8_t dewID)
```
In [Embedded Starter kit](https://www.globallogic.com/ua/embedded-starter-kit/), the default address is 0x80, and can`t change it.
PWM controller has an external clock pin up to 50MHz (max) and an internal clock source 25MHz, in [Embedded Starter kit](https://www.globallogic.com/ua/embedded-starter-kit/) external pin is not used, we can use only internal clock generator. If need to use an external pin just define your new clock generator:
```C
#define OSC_CLOCK 25000000 // osc clock internal
```
And you can change the range of 1% PCA9685 (12bit value 4096) is 40.96
```c
#define one_percent 40.96
```
# Addition function
In [main.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetal06/Core/Src/main.c "main.c") i write function **Process_Command(char*command)**  it is called when something comes into UART buffer, and all symbols are written in **cmd[]**. Then **Process_Command(char*command)** analyze and find existing symbols (name of a function) in an array,  for find I use *strcmp*, and find how many arguments in the function I used *sscanf* it separated arguments by space symbol (%63s). Then check if arguments are not out ob bound and print *OK* or *argument out of bounds/unknown argument/command unknown*

# Function
## void _pwm_set_freq(uint16_t freq) 
This function set PWM frequency (range 24-1524Hz) more info [7.3.5 PWM frequency PRE_SCALE](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK)
But when I test this controller I see the frequency deviate when the frequency set >1000Hz, for example, if the frequency is set 1524 in the output you have ~1600Hz ( I test it only with an internal clock generator.) 
## uint8_t pwm_writefunc(uint8_t PIN, uint16_t HIGH, uint16_t LOW)
This function can set up turn on and turn off time off in PWM, PIN can be range 0-15, often used to write determined coefficients. Value of **HIGH** and **LOW** must bean in the range 0-4095 if someone's value is out of bounds, function return 1,2, or 3 if all correct returned 0. better to use this function if you have already determined the coefficient and need write it. For [example](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK) from datasheet p17:
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
If we do not want every time to determine this coefficients, just use this function
## void _pwm_set_duty_delay(uint8_t PIN, uint8_t duty_cycle, uint8_t start_delay_percent) 
This simple usage function can select duty_cycle **duty cycle** in percent,  startup delay in percent **start_delay_percent**, and select pin **PIN** rage (0-15)
It function has two algorithms to determine coefficients:
*if start_delay_percent + dyty_cycle>100%, duty_cycle has range 0-99% and start_delay_percent 0-100%
if start_delay_percent + dyty_cycle<=100% duty_cycle has range 0-100% and start_delay_percent 0-100%*
more info [here](https://drive.google.com/drive/folders/1EX2lLyHzzsI620431nuTM9JZlnBRSqTK) on p16.
For example  pin 5, duty 30, start delay 0:
```C
_pwm_set_duty_delay(5,30,0);
```
## void _pwm_sleep(STATE status)
This function turns off the oscillator and output pin of PWM controller, used in setup frequency, reset, select clock source function, or power save mode.   (After using **wake** in this function, need manually setup the frequency, else frequency is 200Hz default.) Function catch one of this argument **sleep**, or **wake** 

## void _pwm_out_trigger(TRIGG status) 
Used to turn on/off all pwm pin or change polarity(OUTNE bits), catch one of these arguments **OFF /ON** 
The active OFF output enable (OE) pin, allows to enable or disable all the LED outputs at
the same time.
• When a OFF level is applied to OE pin, all the LED outputs are enabled and follow
the output state defined in the LEDn_ON and LEDn_OFF registers with the polarity
defined by INVRT bit (MODE2 register).
• When a HIGH level is applied to OE pin, all the LED outputs are programmed to the
value that is defined by OUTNE[1:0] in the MODE2 register.
# How to run an example
-   Compile project in CubeIde and load program to MCU,
-  Connect 12v or 5v power supply, to board and connect  LED to P3 connector and ground P5.
-   connect UART-USB connector,
-   open COM terminal [YAT](https://sourceforge.net/projects/y-a-terminal/) terminal for example, select your COM port, setup terminal baud rate 115200, 8bit, no parity, 1 stop bit.
-   open the session and press a black button on Discovery board, if all ok you see the message
>comand list:
>trigger <on/off>
>pulse <0-15> <0-100> (PIN Duty cycle)
>freqmod <24-1526>
>slepmod <off/on>

then you can write one of these commands and show the result
if the function value ok you see a message 
>OK

else 
>unknown argument 

or
>command unknown

or 
>argument out of bounds

[![example test video]({https://drive.google.com/file/d/11ZoxVJghhQJIToPi-CTzMsDcrM4N0YYp/view?usp=share_link})]({https://drive.google.com/file/d/11ZoxVJghhQJIToPi-CTzMsDcrM4N0YYp/view?usp=share_link} "Link Title")
