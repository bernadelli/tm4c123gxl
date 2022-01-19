/*
 * Lesson A9: Push Button delay_debounce [PF0]; Led [PF2]
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
    static char flag = 0;
    SetSystemClock(SYSCTL_SYSDIV_63_5);//Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= CLK_GPIOF;    //Enable clock for PORTF
    GPIO_PORTF_LOCK_R = 0x4C4F434B;    //Unlock GPIOCR register to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTF_CR_R = PF0;             //Enable GPIOPUR register enable to commit PF[0] (Table 10-1. GPIO Pins With Special Considerations)
    GPIO_PORTF_PUR_R |= PF0;           //Enable Pull Up resistor [PF0]
    GPIO_PORTF_DEN_R  = PF0 + PF2;     //Enable [PF2] and [PF0]  - GPIO_PORTF_DEN_R  = 0x5
    GPIO_PORTF_DIR_R  = PF2;           //Input: [PF0] - Output [PF2]

    while (1)
    {
        if((GPIO_PORTF_DATA_R & PF0) == 0) //TRUE when press SW1
        {
            delay(10000);
            if((flag == 0) && (GPIO_PORTF_DATA_R & PF0))
            {
                GPIO_PORTF_DATA_R ^= (PF2);//Toggle PF[2]
                flag = 1;
            }
        }
        else
        {
            flag = 0;
        }
    }
}

