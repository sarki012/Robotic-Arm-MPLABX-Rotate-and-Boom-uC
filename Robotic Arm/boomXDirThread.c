/*
 * File:   xDirThread.c
 * Author: Erik Sarkinen
 *
 * Created on May 22nd, 2023, 3:13 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"



volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20]; 

//volatile extern double boom;
//The DMA puts the ADC value in bufferA
volatile extern __eds__ unsigned int bufferA[SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

void boomXDirThread( void *pvParameters )
{
    int  i = 0, j = 0, stick = 0;
    double x = 4.5;
    double xPrev = 4.5;
    unsigned int boom = 0;
    int numDelayLoops = 4000;       //Was 500 2000 1000 500 2000
    int alpha = 0, alphaFB = 0;
    int breakFlag = 0;
    PHASE2 = 36850;
    PDC2 = 2500;
  
    //PHASE2 = 62500;       //For 120 Mhz with divide by 16 prescalar
    //PDC2 = 10000;
   //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
        breakFlag = 0;
        boom = bufferA[0];    //bufferA is an array that holds the DMA ADC values
        sendCharUart2('z');
        intToCharUart2((int)boom);
        for(i = 0; i < 15; i++)
        {
            if(rxval[i] == 'a')
            {
                stick = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
            }
            else if(rxval[i] == 'x')
            {
                x = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                x /= 10;
                if(x < 4 || x > 10)
                {
                    x = xPrev;
                }
                if(x - xPrev == 0)
                {
                    breakFlag = 1;
                }
                break;
            }
        }
        if(breakFlag != 1)
        {
            alpha = (int)((acos(x/12))*180/3.1416);
            alphaFB = (int)((610 - boom)/3 + 25);
            for(i = 0; i < 20; i++)
            {
                if(usbRxval[i] == '$')      //Stop motors
                {
                    break;
                }
                else if(usbRxval[i] == 'l')     //'O' for out
                {
                    j = 0;
                    while(alphaFB > alpha)
                    {
                        boom = bufferA[j];
                        j++;
                        if(j >= 8)
                        {
                            j = 0;
                        }
                        alphaFB = (int)((610 - boom)/3 + 25);
                        PDC2--;         //Decrementing PDC2 moves the boom down
                        delay(numDelayLoops);
                        if(PDC2 < 1658)
                        {
                            PDC2 = 1658;        //We don't let PDC2 get less than 1658
                        }
                    }
                    j = 0;
                    while(alphaFB < alpha)
                    {
                        boom = bufferA[j];
                        j++;
                        if(j >= 8)
                        {
                            j = 0;
                        }
                        alphaFB = (int)((610 - boom)/3 + 25);
                        PDC2++;         //Incrementing PDC2 moves the boom up
                        delay(numDelayLoops);
                        if(PDC2 > 3870)
                        {
                            PDC2 = 3870;        //We don't let PDC2 get greater than 3870
                        }            
                    }
                    xPrev = x;
                    break;
                }
                else if(usbRxval[i] == 'r')         //I for In
                {
                  //  while((x - xPrev) == 0);
                    j = 0;
                    alpha = (int)((acos(x/12))*180/3.1416);
                    while(alphaFB > alpha)
                    {
                        boom = bufferA[j];
                        j++;
                        if(j >= 8)
                        {
                            j = 0;
                        }
                        alphaFB = (int)((610 - boom)/3 + 25);
                        PDC2--;         //Decrementing PDC2 moves the boom down
                        delay(numDelayLoops);
                        if(PDC2 < 1658)
                        {
                            PDC2 = 1658;        //We don't let PDC2 get less than 1658
                        }
                    }
                    j = 0;
                    while(alphaFB < alpha)
                    {
                        boom = bufferA[j];
                        j++;
                        if(j >= 8)
                        {
                            j = 0;
                        }
                        alphaFB = (int)((610 - boom)/3 + 25);
                        PDC2++;         //Incrementing the duty cycle moves the boom up
                        delay(numDelayLoops);
                        if(PDC2 > 3870)
                        {
                            PDC2 = 3870;        //We don't let PDC2 get greater than 3870
                        }             
                    }
                    xPrev = x;
                    break;
                }
            }
        }
    }
}

