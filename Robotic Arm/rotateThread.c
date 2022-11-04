/*
 * File:   rotateThread.c
 * Author: Erik Sarkinen
 *
 * Created on September 10th, 2022, 1:20 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile extern char usbRxval[50];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void rotateThread( void *pvParameters )
{
    int rightFlag = 0, leftFlag = 0;
    LATAbits.LATA4 = 0;     //Enable (0 = On)
  // LATBbits.LATB0 = 1;     //Direction
    //PHASE1 = 10000;
    //PDC1 = PHASE1/2;
    int  i = 0;
    while(1)
    {
      for(i = 0; i < 45; i++)
        {
            if(usbRxval[i] == 'r')
            {
                //Rotate right
                LATBbits.LATB0 = 1;     //Direction
                PHASE1 = 10000;
                PDC1 = PHASE1/2;
            }
            else if(usbRxval[i] == 'l')
            {
                //Rotate left
                LATBbits.LATB0 = 0;     //Direction
                PHASE1 = 10000;
                PDC1 = PHASE1/2;
            }
            else if(usbRxval[i] == '#')
            {
               leftFlag = 1; 
            }
            else if(usbRxval[i] == '$')
            {
               rightFlag = 1; 
            }
            if(leftFlag == 1 && rightFlag == 1)
            {
                leftFlag = 0;
                rightFlag = 0;
                PHASE1 = 6000;
                PDC1 = 0;
                break;
            }
        }
           
    }
}