/*
 * Lesson K1: Analog Comparator
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

    SYSCTL_RCGCACMP_R |= 0x01;                  // enable Analog Comparator clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOC;    // enable Port C clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;    // enable Port F clock

    GPIO_PORTF_DEN_R |= 0x0E;                   // enable digital I/O on PF1, PF2 and PF3
    GPIO_PORTF_DIR_R |= 0x0E;                   // make PF1 , PF2 and PF3 output
    GPIO_PORTF_AFSEL_R |= 0x02;                 // enable alternate function on PF1
    GPIO_PORTF_PCTL_R = 0x00000200;             // configure PF1 for Analog comparator

    GPIO_PORTC_DEN_R &= ~0x20;                  // disable digital I/O on PC5 - Analog function
    GPIO_PORTC_DIR_R |= 0x20;                   // make PC5 input
    GPIO_PORTC_AFSEL_R |= 0x20;                 // enable alternate function on PC5
    GPIO_PORTC_AMSEL_R |= 0x20;                 // enable analog function

    COMP_ACREFCTL_R = 0x0000000E;
    COMP_ACCTL1_R = 0x0000020C;
    delay(time_delay);


    while(1){
        if(COMP_ACSTAT1_R==0x02)
            GPIO_PORTF_DATA_R = 0x04;

        else
            GPIO_PORTF_DATA_R = 0x08;



      }
}

