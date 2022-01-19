/*
 * Lesson A1: Turn on the red led using "tm4c123gh6pm.h"
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jan/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

void main(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for PORTF
    GPIO_PORTF_DEN_R  = 0x0E;  // Enable PORTF [PF3:PF1] as digital pins
    GPIO_PORTF_DIR_R  = 0x0E;  // Configure PORTF [PF3:PF1] as digital output pins

    while (1) {

        GPIO_PORTF_DATA_R |= 0x04; // [PF2] turn on LED_B
    }
}

