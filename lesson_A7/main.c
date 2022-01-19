/*
 * Lesson A7: Push Button [PF4] and [PF0]; Led [PF2]
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
    unsigned int state;
    SetSystemClock(SYSCTL_SYSDIV_2_5);  //Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= 0x20;          //Enable clock for PORTF
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     //Unlock GPIOCR register to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTF_CR_R = 0X01;             //Enable GPIOPUR register enable to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTF_PUR_R |=0x11;            //Enable Pull Up resistor [PF4] and [PF0]
    GPIO_PORTF_DEN_R  = 0x15;           //Enable [PF4], [PF2] and [PF0]
    GPIO_PORTF_DIR_R  = 0x0E;           //Input: [PF4] and [PF0] - Output [PF3:PF1]

    while (1) {

        state = GPIO_PORTF_DATA_R;            //Read data from PORTF
        state = ~state;                       //Switch is low active; LED is high active
        if(state & PF4 || state & PF0) {      //SW1 [PF4] or SW0 [PF0]
            GPIO_PORTF_DATA_R = PF2;          //Put it on the  LED_B
            delay(time_delay);
            GPIO_PORTF_DATA_R = 0x11;         //Initial value of DATA: [PF4] and [PF0] high

        }

    }
}

