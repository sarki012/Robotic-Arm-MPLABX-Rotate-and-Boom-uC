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

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20];  
volatile extern double boomAvg;

void boomThread( void *pvParameters )
{
    int  i = 0, direction = 0, stick = 0;
    int numDelayLoops = 500;       //Was 2000
    PHASE2 = 36850;         //Fosc = 120 MHz, Prescaler = 8, PHASE2 = 50,000
    PDC2 = 3500;           //Min PDC2 = 16,636, Max PDC2 = 31,818
   //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
        sendCharUart2('z');
        intToCharUart2((int)boomAvg);
        for(i = 0; i < 15; i++)
        {
            if(rxval[i] == 'a')
            {
                stick = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                break;
            }
        }
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '@')
            {
                break;
            }
            else if(usbRxval[i] == 'd')
            {
                PDC2--;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC2 < 1700)
                {
                    PDC2 = 1700;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'u')
            {
                PDC2++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC2 > 3500)
                {
                    PDC2 = 3500;        //We don't let PDC2 get greater than 3870
                } 
            }
        }
    }
}

/*        if(direction == 0)
        {
            PDC2--;
            delay(numDelayLoops);
            if(PDC2 < 1700)
            {
                direction = 1;
            }
        }
        else if(direction == 1)
        {
            PDC2++;
            delay(numDelayLoops);
            if(PDC2 > 3500)
            {
                direction = 0;
            }
        }*/
          /*
        if(direction == 0)
        {
            PDC2--;
            delay(numDelayLoops);
            if(PDC2 < 10000)
            {
                direction = 1;
            }
        }
        else if(direction == 1)
        {
            PDC2++;
            delay(numDelayLoops);
            if(PDC2 > 15000)
            {
                direction = 0;
            }
        }
        */