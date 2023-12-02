/*
 * File:   rotateAutoThread.c
 * Author: Erik Sarkinen
 *
 * Created on November 29th, 2023, 5:42 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void rotateAutoThread( void *pvParameters )
{
    int rightFlag = 0, leftFlag = 0;
    LATAbits.LATA4 = 0;     //Enable (0 = On)
  //  LATBbits.LATB0 = 1;     //Direction
    PHASE1 = 10000;
    PDC1 = 0;
    int  i = 0;
    while(1)
    {
        for(i = 0; i < 20; i++)
        { 
            for(i = 0; i < 20; i++)
            {
                if(usbRxval[i] == 'q')
                {
                    PHASE1 = 6000;
                    PDC1 = 0;
                    break;
                }
                else if(usbRxval[i] == '}')     //Right
                {
                    //Rotate right
                    LATBbits.LATB0 = 0;     //Direction
                    //PHASE1 = 65534;
                    //PHASE1 = 36850;
                    //PHASE1 = 40000;
                    PHASE1 = 65534;
                    PDC1 = PHASE1/2;
                    rightFlag = 1;
                }
                else if(usbRxval[i] == '{')     //Left
                {
                    //Rotate left
                    LATBbits.LATB0 = 1;     //Direction
                    PHASE1 = 65534;
                    PDC1 = PHASE1/2;
                    leftFlag = 1;
                }
            }
        }   
    }
}