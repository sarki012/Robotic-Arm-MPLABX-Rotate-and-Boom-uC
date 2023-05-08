/*
 * File:   .c
 * Author: Erik Sarkinen
 *
 * Created on March 29th, 2023 2:22 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

#define bufferSize 10
volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 

//The DMA puts the ADC value in bufferA
volatile extern __eds__ unsigned int bufferA[SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));
volatile extern double boomAvg;

void feedbackThread( void *pvParameters )
{
    int i = 0, count = 0;
    unsigned long int boomAvgBuffer = 0;
    while(1)
    {
        boomAvgBuffer += bufferA[i];    //bufferA is an array that holds the DMA ADC values
        delay(500);
        count++;
        i++; 
        //Only going to send the average every bufferSize loops of the thread
        if(count == bufferSize)
        {
            boomAvg = boomAvgBuffer/bufferSize;       //Average
            sendCharUart1('b');      //Send 'b' for boom
            intToCharUart1((int)boomAvg);       //intToChar converts the integer to four characters and sends them out on UART1
            boomAvgBuffer = 0;
            count = 0;
            
        } 
              //Increment by 2 because data is in words
        if(i >= 8)
        {
            i = 0;
        }
    }
}
