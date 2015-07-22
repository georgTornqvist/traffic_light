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

int main(void)
{
		//
	    // Run from the PLL at 120 MHz.
	    //
	    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
	                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
	                SYSCTL_CFG_VCO_480), 120000000);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

	GPIOPinTypeGPIOOutput(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen);
	GPIOPinTypeGPIOOutput(PED_LIGHT_PORT,  pedGreen|pedRed);


	GPIOPinWrite(MAIN_LIGHT_PORT, mainRed|mainYellow|mainGreen, 0x00);		// assure the red and blue LEDs are off
	GPIOPinWrite(PED_LIGHT_PORT,  pedGreen|pedRed, 0x00);


	while(1)
	{
		goSignal();

		delayMS(10000);

		stopSignal();

		delayMS(5000);

	}
}


