//****************************************************************************
/*
 *   Filename:      TrafficLight/main.c
 *   Date:          30/07/2015
 *   File Version:  0.1
 *
 *   Author:        Giorgio Manenti
 *   Company:
 *
 ******************************************************************************
 *
 *   Architecture:  TM4C1294XL
 *   Processor:     TM4C1294NCPDT
 *   Compiler:      Code Composer Studio  Version: 6.1.0.00104
 *
 ******************************************************************************
 *
 *   Files required: none
 *
 ******************************************************************************
 *
 *   Description:
 *
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"


#define MAIN_LIGHT_PORT  	GPIO_PORTF_BASE
#define PED_LIGHT_PORT		GPIO_PORTG_BASE

#define mainRed				GPIO_PIN_1
#define mainYellow			GPIO_PIN_2
#define mainGreen			GPIO_PIN_3

#define pedRed				GPIO_PIN_0
#define pedGreen			GPIO_PIN_1

#define timer0val			5
#define timer1val			3
#define timer2val			2

//*****************************************************************************

// The error routine that is called if the driver library encounters an error.

//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
	UARTprintf(*pcFilename, ui32Line);
}
#endif


//*****************************************************************************
// System clock rate in Hz.
//*****************************************************************************
uint32_t g_ui32SysClock;

// *****************************************************************************
// DELAY FUNCTION
// *****************************************************************************
void delayMS(int ms) {
    //delay funct
    SysCtlDelay( (g_ui32SysClock/(3*1000))*ms ) ;
}

// *****************************************************************************
// CAR GREEN LIGHT, PED RED LIGHT
// *****************************************************************************
void goSignal(){
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
	UARTprintf("go \n");
}

// *****************************************************************************
// PEDESTRIAN CALL
// *****************************************************************************
void button1IntHandler(){
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x04);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
	TimerEnable(TIMER0_BASE, TIMER_A);
	GPIOIntDisable(GPIO_PORTJ_BASE,GPIO_PIN_0); //disable the button inrterrupt
	GPIOIntClear(GPIO_PORTJ_BASE,GPIO_PIN_0);
	UARTprintf("PEDESTRIAN call \n");
}

// *****************************************************************************
// CAR YELLOW LIGHT, PED RED LIGHT 5sec
// *****************************************************************************
void Timer0IntHandler(){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x02);
	TimerEnable(TIMER1_BASE, TIMER_A);
	UARTprintf("Slow Down \n");
}
// *****************************************************************************
// CAR RED LIGHT, PED GREEN LIGHT BLINKING 2sec
// *****************************************************************************
void Timer1IntHandler(){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x02);
	delayMS(500);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x00);
	TimerEnable(TIMER2_BASE, TIMER_A);
	UARTprintf("Hurry up John! \n");
}

// *****************************************************************************
// CAR GREEN LIGHT, PED RED LIGHT 2sec
// *****************************************************************************
void Timer2IntHandler(){
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
	GPIOIntEnable(GPIO_PORTJ_BASE,GPIO_PIN_0); //enable the button inrterrupt
	UARTprintf("Don't Walk! \n");
}

// *****************************************************************************
// Configure the UART and its pins.  This must be called before UARTprintf().
// *****************************************************************************
void
ConfigureUART(void){
    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}


int main(void)
{
	// *****************************************************************************
	// Clock config to run at 120 MHz
	// *****************************************************************************

	g_ui32SysClock = SysCtlClockFreqSet((	SYSCTL_XTAL_25MHZ |
						SYSCTL_OSC_MAIN   |
						SYSCTL_USE_PLL 	  |
						SYSCTL_CFG_VCO_480), 120000000);

	// *****************************************************************************
	// Configure the UART
	// *****************************************************************************
	ConfigureUART();

	// *****************************************************************************
	// Peripherals enable
	// *****************************************************************************
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    	// *****************************************************************************
    	// Peripherals config
    	// *****************************************************************************

	 //GPIO leds
	GPIOPinTypeGPIOOutput(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen);
	GPIOPinTypeGPIOOutput(PED_LIGHT_PORT,  pedGreen|pedRed);

	// assure all LEDs are off
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x00);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x00);

	//GPIO buttons
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);

	 // Set each of the button GPIO pins as an input with a pull-up.
    	GPIODirModeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);
    	GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


	// *****************************************************************************
	// Timers config
	// *****************************************************************************

	// Configure the three one shot timers.
	TimerConfigure(TIMER0_BASE, TIMER_CFG_A_ONE_SHOT);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_A_ONE_SHOT);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_A_ONE_SHOT);

	// Set the count time for the the one-shot timers.
	TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock*timer0val);
	TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock*timer1val);
	TimerLoadSet(TIMER2_BASE, TIMER_A, g_ui32SysClock*timer2val);

	// *****************************************************************************
	// Interrupts config for user switch
	// *****************************************************************************

	//button (user switch 1) interrupts triggered on falling edge
	GPIOIntEnable(GPIO_PORTJ_BASE,GPIO_PIN_0);
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
	//register button interrupt
	GPIOIntRegister(GPIO_PORTJ_BASE, button1IntHandler);


	// *****************************************************************************
	// Setup the interrupts for the timer timeouts.
	// *****************************************************************************
	IntEnable(INT_TIMER0A);
	IntEnable(INT_TIMER1A);
	IntEnable(INT_TIMER2A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	// *****************************************************************************
	// Enable processor interrupts
	// *****************************************************************************
    	IntMasterEnable();

    //cycle forever and ever
	while(1)
	{

	}
}


