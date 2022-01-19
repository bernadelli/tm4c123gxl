/*
 * Lesson A8: Push Button toggle led [PF4:PF1]
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

void main(void)
{
    unsigned int state;
    SetSystemClock(SYSCTL_SYSDIV_63_5); ///Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= 0x20;          // Enable clock for PORTF
    GPIO_PORTF_PUR_R |=0x10;            // Enable Pull Up resistor [PF4]
    GPIO_PORTF_DEN_R  = 0x18;           // Enable [PF4] and [PF3]
    GPIO_PORTF_DIR_R  = 0x08;           // Input: [PF4] - Output [PF3]

    while (1) {

        state = GPIO_PORTF_DATA_R & PF4;
        GPIO_PORTF_DATA_R = (~state>>1); //1 to LED_G; 2 to LED_B; 3 to LED_R (according DEN and DIR)

    }
}

