/*
 * File:   boomThread.c
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
#include <stdio.h>
#include <stdlib.h>

volatile extern char usbRxval[5];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void boomThread( void *pvParameters )
{
    int  i = 0, direction = 1;
    int numDelayLoops = 500;
 
    PHASE2 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC2 = 3870;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    while(1)
    {
        for(i = 0; i < 5; i++)
        {
            if(usbRxval[i] == 'u')
            {
                PDC2 --;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC2 < 1658)
                {
                    PDC2 = 1658;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'd')
            {
                PDC2 ++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC2 > 3870)
                {
                    PDC2 = 3870;        //We don't let PDC2 get greater than 3870
                } 
            }
            
        }
    }
}
  
        /*
   */
             /*
        if (direction == 1)
        {
            PDC2++;
            delay(1000);
            if(PDC2 > 3870)
            {
                direction = 0;
            }
        }
        else if(direction == 0)
        {
            PDC2--;
            delay(1000);
            if(PDC2 < 1658)
            {
                direction = 1;
            }
        }
         */ 