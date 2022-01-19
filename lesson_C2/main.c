/*
 * Lesson C2: TimerA interrupt  - Toggle BLED [PF3]
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version mar/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"

#define MS 1000
#define SIZE 1024
unsigned char Buffer[SIZE];

int main(void)
{

    //unsigned int tt;
    SetSystemClock(SYSCTL_SYSDIV_60);//Parameters from sysctl.h (TivaWare Parameter)
    /*Iniitialize PF3 as a digital output pin */
   SYSCTL_RCGCGPIO_R |= 0x20; // turn on bus clock for GPIOF
   //tt=0;
   GPIO_PORTF_DIR_R  |= PF2; // set blue pin as a digital output pin
   GPIO_PORTF_DEN_R  |= PF2;  // Enable PF2 pin as a digital pin

   Time5A_Init(64000);
   while(1)
    {
        // do nothing wait for the interrupt to occur

    }
}
