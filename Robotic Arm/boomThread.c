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
#include <math.h>

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20];  
//volatile extern double boom;
//The DMA puts the ADC value in bufferA
volatile extern __eds__ unsigned int bufferA[SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));


void boomThread( void *pvParameters )
{
    int  i = 0, direction = 0, stick = 0;
    int numDelayLoops = 3000;       //Was 2000
   // PHASE2 = 62500;
    //PDC2 = 10000;
    PHASE2 = 36850;         //Fosc = 120 MHz, Prescaler = 8, PHASE2 = 50,000
    PDC2 = 2500;           //Min PDC2 = 16,636, Max PDC2 = 31,818
   //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '@')
            {
                break;
            }
            else if(usbRxval[i] == 'd')
            {
                PDC2--;         //Decrementing the duty cycle moves the boom down
                delay(numDelayLoops);
                if(PDC2 < 1658)
                {
                    PDC2 = 1658;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'u')
            {
                PDC2++;         //Incrementing the duty cycle moves the boom up
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
void boomThread( void *pvParameters )
{
    int  i = 0, j = 0, stick = 0;
    unsigned int boom = 0;
    int numDelayLoops = 4000;       //Was 500 2000 1000 500 2000
    double x = 0;
    int alpha = 0, alphaFB = 0;
    PHASE2 = 36850;         //Fosc = 120 MHz, Prescaler = 8, PHASE2 = 50,000
    PDC2 = 2200;           //Was 3500. Min PDC2 = 16,636, Max PDC2 = 31,818
   //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
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
                x /= 100;
                break;
            }
        }
        alpha = (int)((acos(x/12))*180/3.1416);
       // alphaFB = (int)((600 - boom)/3 + 27);
        alphaFB = (int)((600 - boom)/3 + 25);
        //alpha = acos(x/12);
        //alphaFB = (((600 - boom)/3)*3.1416)/180 + .47;
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '&')      //was '@'
            {
                break;
            }
            else if(usbRxval[i] == 'O')     //'O' for out
            {
                j = 0;
               // alpha = acos(x/12);
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
                  //  alphaFB = (((600 - boom)/3)*3.1416)/180 + .47;
                    PDC2--;         //Decrementing PDC2 moves the boom down
                    delay(numDelayLoops);
                    if(PDC2 < 1500)
                    {
                        PDC2 = 1500;        //We don't let PDC2 get less than 1658
                        break;
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
                 //   alphaFB = (((600 - boom)/3)*3.1416)/180 + .47;
                    PDC2++;         //Incrementing PDC2 moves the boom up
                    delay(numDelayLoops);
                    if(PDC2 > 3500)
                    {
                        PDC2 = 3500;        //We don't let PDC2 get greater than 3870
                        break;
                    }            
                }
                break;
                
                //PDC2--;         //Decrementing the duty cycle moves the boom down
                //delay(numDelayLoops);
                //if(PDC2 < 1700)
               // {
                 //   PDC2 = 1700;        //We don't let PDC2 get less than 1658
               // }
                  
            }
            else if(usbRxval[i] == 'I')         //I for In
            {
                j = 0;
                alpha = (int)((acos(x/12))*180/3.1416);
               // alpha = acos(x/12);
                while(alphaFB > alpha)
                {
                    boom = bufferA[j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    alphaFB = (int)((610 - boom)/3 + 25);
                  //  alphaFB = (((600 - boom)/3)*3.1416)/180 + .47;
                    PDC2--;         //Decrementing PDC2 moves the boom down
                    delay(numDelayLoops);
                    if(PDC2 < 1500)
                    {
                        PDC2 = 1500;        //We don't let PDC2 get less than 1658
                        break;
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
                 //   alphaFB = (((600 - boom)/3)*3.1416)/180 + .47;
                    PDC2++;         //Incrementing the duty cycle moves the boom up
                    delay(numDelayLoops);
                    if(PDC2 > 3500)
                    {
                        PDC2 = 3500;        //We don't let PDC2 get greater than 3870
                        break;
                    }            
                }
                break;
                
                //PDC2++;         //Incrementing the duty cycle moves the boom up
                //delay(numDelayLoops);
                //if(PDC2 > 3500)
                //{
                  //  PDC2 = 3500;        //We don't let PDC2 get greater than 3870
                //} 
                  
            }
        }
    }
}
*/
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