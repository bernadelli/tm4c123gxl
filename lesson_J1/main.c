/*
 * Lesson J1: watchdog
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

#define time_delay 500000

void main(void)
{

    //Parameters from sysctl.h (TivaWare Parameter)
    SetSystemClock(SYSCTL_SYSDIV_60);

    SYSCTL_RCGCWD_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;    // enable Port F clock
    GPIO_PORTF_DIR_R |= 0x0E;                   // make PF3-1 output (PF3-1 built-in LEDs)
    GPIO_PORTF_AFSEL_R &= ~0x0E;                // disable alt funct on PF3-1
    GPIO_PORTF_DEN_R |= 0x0E;                   // enable digital I/O on PF3-1 - configure PF3-1 as GPIO

    WATCHDOG0_LOAD_R = 0xFFFFFF;
    WATCHDOG0_CTL_R |= 0x01;

    while(1){
        if(WATCHDOG0_RIS_R==1){
            GPIO_PORTF_DATA_R = RED;
            WATCHDOG0_ICR_R |= 0xFF;
            delay(time_delay);
        }
        GPIO_PORTF_DATA_R = BLUE;

      }
}

