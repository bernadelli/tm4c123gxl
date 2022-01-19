/*
 * Lesson A6: Push Button [PF4] and [PF1]
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
    SetSystemClock(SYSCTL_SYSDIV_63_5); //Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= 0x20;          // Enable clock for PORTF
    GPIO_PORTF_PUR_R |=0x10;            // Enable Pull Up resistor [PF4]
    GPIO_PORTF_DEN_R  = 0x12;           // Enable [PF4] and [PF1]
    GPIO_PORTF_DIR_R  = 0x0E;           // Input: [PF4] and [PF0] - Output [PF3:PF1]

    while (1) {

        state = GPIO_PORTF_DATA_R;   //Read data from PORTF
        state = ~state;              //Switch is low active; LED is high active
        if(state & PF4){             //SW1 [PF4]
            GPIO_PORTF_DATA_R = PF1; //Put it on the  LED_R
        }
    }
}

