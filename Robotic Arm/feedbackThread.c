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
#include "math.h"

#define bufferSize 10
volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 

//The DMA puts the ADC value in bufferA
//volatile extern __eds__ unsigned int bufferA[SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));
volatile extern double claw;
volatile extern int clawFB[15];
volatile extern int sendFlag;
void feedbackThread( void *pvParameters )
{
    int i = 0, count = 0;
  //  sendCharUart1('c');      //Send 'c' for claw
   // intToCharUart1((int)clawFB);       //intToChar converts the integer to four characters and sends them out on UART1
    while(1)
    {
    //    if(abs clawFB - clawFBPrev)
        if(sendFlag == 1)
        {
            for(i = 0; i < 15; i++)
            {
               sendCharUart1('c');      //Send 'c' for claw
               intToCharUart1((int)clawFB[i]);       //intToChar converts the integer to four characters and sends them out on UART1
               count = 0;
            }
            sendFlag = 0;
        }
    }
}
        /*
        if(count != 1000)
        {
            clawFB = bufferA[0];
            sendCharUart1('c');      //Send 'c' for claw
            intToCharUart1((int)bufferA[i]);       //intToChar converts the integer to four characters and sends them out on UART1
            count = 0;
        }
         */ 
      ///  count++;
        /*
        clawFB = bufferA[i];
        if(abs(clawFB - clawFBPrev) > 10)
        {
            sendCharUart1('c');      //Send 'c' for claw
            intToCharUart1((int)bufferA[i]);       //intToChar converts the integer to four characters and sends them out on UART1
            clawFBPrev = clawFB;
        }
        if(i == SAMP_BUFF_SIZE)
        {
            i = 0;
        }
        i++; 
         */ 
        /*
        clawAvgBuffer += bufferA[i];    //bufferA is an array that holds the DMA ADC values
        delay(500);
        count++;
        i++; 
        //Only going to send the average every bufferSize loops of the thread
        if(count == bufferSize)
        {
            sendCharUart1('c');      //Send 'c' for claw
            intToCharUart1((int)clawAvgBuffer);       //intToChar converts the integer to four characters and sends them out on UART1
            clawAvgBuffer = 0;
            count = 0;
            
        } 
        if(i >= 8)
        {
            i = 0;
        }
         * */
 //   }
//}
