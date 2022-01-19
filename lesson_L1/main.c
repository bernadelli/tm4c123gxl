/*
 * Lesson L1: QEI
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jan/2022  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"

#define time_delay 100000

void main(void)
{

    //Parameters from sysctl.h (TivaWare Parameter)
    SetSystemClock(SYSCTL_SYSDIV_60);

    SYSCTL_RCGCQEI_R |= 0x01;                  // enable Analog Comparator clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOD;    // enable Port D clock
    //SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;    // enable Port F clock
    delay(time_delay);
    GPIO_PORTD_LOCK_R = 0x4C4F434B;            //Unlock GPIOCR register to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTD_CR_R = 0XFF;                    //Enable GPIOPUR register enable to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTD_DIR_R &= ~0xC8;                   // make PD3, PD6 and PD7 input
    GPIO_PORTD_DEN_R |= 0x0C8;                 // enable digital I/O on PD3, PD6 and PD7
    GPIO_PORTD_AFSEL_R |= 0xC8;                // enable alternate function on PD3, PD6 and PD7
    GPIO_PORTD_PCTL_R = 0x00000040;            // configure PD3, PD6 and PD7 for QEI

    QEI0_CTL_R = 0x00000018;
    QEI0_MAXPOS_R = 0x00000F9F;
    QEI0_CTL_R |= 0x01;


    while(1){



      }
}

