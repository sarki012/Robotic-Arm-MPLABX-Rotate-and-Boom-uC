/*
 * File:   main.c
 * Author: Erik Sarkinen
 *
 * Created on September 10th, 2022, 1:20 PM
 */
// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = OFF              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PWMLOCK = OFF            // PWM Lock Enable bit (PWM registers may be written without key sequence)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

#include <xc.h>
#include <p33ep512mc502.h>
/* Standard includes. */
#include <stdio.h>
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile char usbRxval[20];     //The UART receive array which holds the data sent 
                                //via USB from the Raspberry Pi
volatile char rxval[20];     //The UART receive array which holds the data sent 
                                //via USB from the Raspberry Pi
volatile double claw = 0;
volatile int clawFB[15];
volatile int sendFlag = 0;
volatile int rotateCount = 0;

int x = 0, y = 0, up = 0, down = 0, left = 0, right = 0;

//Uart1 receive interrupt, receiving characters from the Raspberry Pi
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)             
{
    IFS0bits.U1RXIF = 0;        //Clear the interrupt flag
    usbRxval[x] = U1RXREG;         //Add the value in the receive register to the receive array
    x++;
    if(x == 20)
    {  
        x = 0;
    }
    return;
 }
//UART 2 receive interrupt, receiving characters from stick and claw microcontroller
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)             
{
    IFS1bits.U2RXIF = 0;        //Clear the interrupt flag
    rxval[y] = U2RXREG;         //Add the value in the receive register to the receive array
    y++;
    if(y == 20)
    {  
        y = 0;
    }
    return;
 }

void __attribute__ ((interrupt, auto_psv)) _AD1Interrupt(void)   
{
    // AD Conversion complete interrupt handler 
    if(IFS0bits.AD1IF)
    {
        IFS0bits.AD1IF = 0;        // Clear ADC Interrupt Flag  
        clawFB[0] = ADC1BUF0;
        clawFB[1] = ADC1BUF1;
        clawFB[2] = ADC1BUF2;
        clawFB[3] = ADC1BUF3;
        clawFB[4] = ADC1BUF4;
        clawFB[5] = ADC1BUF5;
        clawFB[6] = ADC1BUF6;
        clawFB[7] = ADC1BUF7;
        clawFB[8] = ADC1BUF8;
        clawFB[9] = ADC1BUF9;
        clawFB[10] = ADC1BUFA;
        clawFB[11] = ADC1BUFB;
        clawFB[12] = ADC1BUFC;
        clawFB[13] = ADC1BUFD;
        clawFB[14] = ADC1BUFE;
        sendFlag = 1;
         
    }
    return;                        
}
/*
void __attribute__ ((interrupt, auto_psv)) _PWMInterrupt(void)   
{
    if(PWMCON1bits.TRGSTAT)
    {
        PWMCON1bits.TRGSTAT = 0;
        rotateCount++;
    }
    
    // AD Conversion complete interrupt handler 
    if(PTCONbits.SESTAT)
    {
        PTCONbits.SESTAT = 0;
        rotateCount++;
    }
      
    return;                        
}
*/
void __attribute__((__interrupt__, auto_psv)) _DefaultInterrupt(void)
{
    int k = 0;
    k++;
    return;
}


void main(void) {
    int i = 0;
    for(i = 0; i < 20; i++)
    {
        usbRxval[i] = 0;
        rxval[i] = 0;       //Initialize the receive array to all 0's
    }
    for(int j = 0; j < 15; j++)
    {
        clawFB[j] = 0;
    }
    init();     //Setup clock, UART, and PWMs
 //   initAdc1();
 //   initTmr3();
   // initDma0();
    
    xTaskCreate( rotateThread, "Rotate", 512, NULL, 1, NULL );      //Thread that controls rotation
    xTaskCreate( clawThread, "Claw", 256, NULL, 1, NULL );      //Thread that controls the tip motion
    xTaskCreate( feedbackThread, "Feedback", 512, NULL, 1, NULL );      //Thread that sends the ADC values to the Raspberry PI
    xTaskCreate( rotateAutoThread, "Rotate Auto", 512, NULL, 1, NULL );
   // xTaskCreate( boomXDirThread, "Horizontal", 512, NULL, 1, NULL );        //Thread that controls horizontal kinematics
	//Start the scheduler
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
    return;
}
void vApplicationIdleHook( void )
{
	/* Schedule the co-routines from within the idle task hook. */
	vCoRoutineSchedule();
}