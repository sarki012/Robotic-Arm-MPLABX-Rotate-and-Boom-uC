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

volatile extern char usbRxval[50];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void boomThread( void *pvParameters )
{
    int  i = 0;
    int numDelayLoops = 1000;
    PHASE2 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC2 = 2500;            //Duty cycle register. Starting duty cycle is 2500.
    while(1)
    {
      for(i = 0; i < 45; i++)
        {
            if(usbRxval[i] == 'u')
            {
                PDC2--;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC2 < 1474)
                {
                    PDC2 = 1474;        //We don't let PDC1 get less than 1,474
                }
            }
            else if(usbRxval[i] == 'd')
            {
                PDC2++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC2 > 4054)
                {
                    PDC2 = 4054;        //We don't let PDC1 get greater than 4054
                } 
            }
        }   
    }
}    