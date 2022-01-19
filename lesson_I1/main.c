/*
 * Lesson I1: uDMA Memory to memory
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version dez/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"

#define time_delay 500000
#define SIZE 256
unsigned int SrcBuf[SIZE], DestBuf[SIZE];

void main(void)
{

    int i,t=0;
    volatile unsigned int ddelay;
    const int COLORWHEEL[8] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};


    //Parameters from sysctl.h (TivaWare Parameter)
    SetSystemClock(SYSCTL_SYSDIV_10);


    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;    // enable Port F clock
    ddelay = SYSCTL_RCGCGPIO_R;                 // allow time to finish
    GPIO_PORTF_DIR_R |= 0x0E;                   // make PF3-1 output (PF3-1 built-in LEDs)
    GPIO_PORTF_AFSEL_R &= ~0x0E;                // disable alt funct on PF3-1
    GPIO_PORTF_DEN_R |= 0x0E;                   // enable digital I/O on PF3-1 - configure PF3-1 as GPIO
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
    GPIO_PORTF_AMSEL_R = 0;                     // disable analog functionality on PF

    DMA_Init();                                 // initialize DMA channel 30 for software transfer

    while(1){
        for(i=0;i<SIZE;i++){
            SrcBuf[i] = i;
            DestBuf[i] = 0;
        }
        while(DMA_Status());                    // wait for idle
        DMA_Transfer(SrcBuf,DestBuf,SIZE);
        GPIO_PORTF_DATA_R = COLORWHEEL[t&0x07];
        t += 1;
        delay(time_delay);
      }
}

