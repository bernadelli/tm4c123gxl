/*
 * Lesson A5: Passing clock divisor by value and blink the RGB led
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jan/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


//---USER CONSTANTS---//
#define time_delay 1000000

void main(void)
{
    SetSystemClock(SYSCTL_SYSDIV_63_5); //Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for PORTF
    GPIO_PORTF_DEN_R  = 0x0E;  // Enable PORTF [PF3:PF1] as digital pins
    GPIO_PORTF_DIR_R  = 0x0E;  // Configure PORTF [PF3:PF1] as digital output pins

    while (1) {

        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF1; // turn on LED_R
        delay(time_delay);
        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF2; // turn on LED_B
        delay(time_delay);
        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF3; // turn on LED_R
        delay(time_delay);
    }
}

