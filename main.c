#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"


#define MAIN_LIGHT_PORT  	GPIO_PORTF_BASE
#define PED_LIGHT_PORT		GPIO_PORTG_BASE

#define mainRed				GPIO_PIN_1
#define mainYellow			GPIO_PIN_2
#define mainGreen			GPIO_PIN_3

#define pedRed				GPIO_PIN_0
#define pedGreen			GPIO_PIN_1

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;



void delayMS(int ms) {
    //ROM_SysCtlDelay( (ROM_SysCtlClockGet()/(3*1000))*ms ) ;  // more accurate
    SysCtlDelay( (g_ui32SysClock/(3*1000))*ms ) ;  // less accurate
}


void stopSignal(){
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x04);
	delayMS(5000);
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x02);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x02);
}

void goSignal(){
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
}

void button1IntHandler(){
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x04);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer0IntHandler(){

	GPIOIntDisable(GPIO_PORTJ_BASE,GPIO_PIN_0); //disable the button inrterrupt
	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x02);
	TimerEnable(TIMER1_BASE, TIMER_A);

}

void Timer1IntHandler(){

	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x02);
	delayMS(500);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x00);
	TimerEnable(TIMER2_BASE, TIMER_A);

}
void Timer2IntHandler(){

	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x08);
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x01);
	GPIOIntEnable(GPIO_PORTJ_BASE,GPIO_PIN_0); //enable the button inrterrupt
}

int main(void)
{
	//
	// Run from the PLL at 120 MHz.
	//
	    g_ui32SysClock = SysCtlClockFreqSet((	SYSCTL_XTAL_25MHZ |
	                				SYSCTL_OSC_MAIN   |
							SYSCTL_USE_PLL 	  |
							SYSCTL_CFG_VCO_480), 120000000);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    // Enable the peripherals used by this example.

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);


	GPIOPinTypeGPIOOutput(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen);
	GPIOPinTypeGPIOOutput(PED_LIGHT_PORT,  pedGreen|pedRed);

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);

	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x00);		// assure the red and blue LEDs are off
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x00);

	//interrupts define for User Switch 1
	GPIOIntEnable(GPIO_PORTJ_BASE,GPIO_PIN_0);
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);


    // Configure the three one shot timers.

    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_ONE_SHOT);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_ONE_SHOT);
    TimerConfigure(TIMER2_BASE, TIMER_CFG_A_ONE_SHOT);

	//
	// Set the count time for the the one-shot timer (TimerA).
	//
	TimerLoadSet(TIMER0_BASE, TIMER_A, 5000);
	TimerLoadSet(TIMER1_BASE, TIMER_A, 2000);
	TimerLoadSet(TIMER2_BASE, TIMER_A, 2000);


    // Enable processor interrupts.

    IntMasterEnable();


	while(1)
	{
		goSignal();

	}
}


