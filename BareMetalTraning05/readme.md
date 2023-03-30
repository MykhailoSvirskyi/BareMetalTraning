# Bare Metal Training 05: UART

**Deadline: 06.03.2023**
## Assignment
1.  Need to start UART3 with the following settings: baud rate 115200, 8bit, no parity, 1 stop bit.
    
2.  Need to control all 4 LED states with help of the PC keyboard. Keyboard button toggles LED state (via console). Current LED state printed to the console.    
3.  Need to control all 4 LED states with help of buttons SWT1, SWT3, SWT4, SWT5 (on the GL board) – toggle mode. Current LED state printed to the console.    
4.  One time per 5 seconds, actual temperature (external analog sensor at GL board) value should be printed at console (Celsius degrees).    
5.  [*OPTIONAL]. Create SW at PC to handle functionality that listed above (p. 2, 4) instead of console.
    6.  Use hairdryer to get high temperature for components. Please be careful to save your health and HW equipment. Safety first!!!    
7.  Need to capture signal trace from for outputs with help of signal analyzer to show that all functionality is working fine.  
    Trace should be saved as *.sal file and provided for mentor for analysis.
    
8.  Project need to be stored with your own repository on the GitHub.  
    Provide it to your mentor for analysis and grade.  
    All other methods of transferring files are forbidden.
    
9.  Need to capture video. Store it on a GoogleDrive (YouTube, etc…).  
    Provide a link to it for your mentor.
    

10. Enjoy!!! 😊 <br>

# Functionaluty
You can change debounce time of the button 
```C
#define DEBOUNCE 100
```
And change the period to send the temperature value in the terminal
```C
#define temp_time_every_ms 5000 //send temperature every 5s;
```
We can setup ADC voltage level, ADC extension 
```C
#define ADC_RANGE 4096
#define ADC_VOLT 3.3
#define ADC_STEP (ADC_RANGE/ADC_VOLT)
#define ADC_100C (0.02*ADC_STEP)
#define ADC_0C (2.02*ADC_STEP)
#define ADC_minus_24C (2.52*ADC_STEP)
#define ADC_100C_0C_coef ((ADC_0C)/100)
#define temp_corect 10 //correction temperature
#define ADC_TO_CELSIUS(adc) ((adc)<(ADC_0C))? (100+(temp_corect)-((adc)/(ADC_100C_0C_coef))):(-88)//if ADC<2.02v t=0C, and  adc=0.02v t=99C
//if ADC<2.02v t=0C, and adc=0.02v t=99C
```
To determine temperature/voltage i used *ADC_TO_CELSIUS(adc)* macros it return temperature range 0-99C if temperature <0 return -88;
if need the correct temperature value you can change **temp_corect** to another value, for example, if select 0 this sensor shows 4C when the real temperature is 14C.
In the while loop, I write two simple code that generates a massage about temperature and send it to a terminal,  second function wait when some symbol comes in the UART buffer, then check if it is $ check number and turned LED pin.
A [button.c](https://github.com/MykhailoSvirskyi/BareMetalTraning/blob/master/BareMetalTraning05/Core/Src/button.c "button.c") has two function **LED_STATUS (GPIO_TypeDef*gPIOx, uint16_t gPIO_Pin)** and **_Button_reader (uint16_t GPIO)** 
First, write in the terminal status of GPIO LED pin and toggle LED, second function handle button press.

# How to run this example
-   Compile project in CubeIde and load program to MCU,
-  Connect 12v or 5v power supply, to board and connect  LED to P3 connector and ground P5.
-   connect UART-USB connector,
-   open COM terminal [YAT](https://sourceforge.net/projects/y-a-terminal/) terminal for example, select your COM port, setup terminal baud rate 115200, 8bit, no parity, 1 stop bit.
-   open the session and press a black button on Discovery board, if all done you see the massage:
>togle led pin use $<> 1-4 if all led $5

if you press some button (SWT1-SWT5)  some led turns on/off and in a terminal you show a message about this like:
>BLUE LED ON
>RED LED OFF

if write *$1* in a terminal, blue pin toggle state, and print status like line upper.
every 5 seconds in terminal printed temperature like this:
>temperature =12 C

if the temperature is lower then 0C degrees, the terminal printed, determining negative degree of Celsius not written.
>temperature =-88 C

