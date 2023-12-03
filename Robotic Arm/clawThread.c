/*
 * File:   clawThread.c
 * Author: Erik Sarkinen
 *
 * Created on November 26th, 2022, 8:30 AM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void clawThread( void *pvParameters )
{
    int  i = 0;
    int numDelayLoops = 1000;
 //   PHASE3 = 62500;
   // PDC3 = 10000;
    PHASE2 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC2 = 1800;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    while(1)
    {
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '%')
            {
                break;
            }
            else if(usbRxval[i] == 'n')     //Claw open
            {
                PDC2--;         //Decrementing the duty cycle opens the claw
                delay(numDelayLoops);
                if(PDC2 < 1750)
                {
                    PDC2 = 1750;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'c')     //Claw closed
            {
                PDC2++;         //Incrementing the duty cycle closes the claw
                delay(numDelayLoops);
                if(PDC2 > 2800)
                {
                    PDC2 = 2800;        //We don't let PDC2 get greater than 3870
                } 
            }        
        }
    }
}