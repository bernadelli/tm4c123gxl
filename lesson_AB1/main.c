/*
 * Lesson AB1: GPIO Interrupt - Toggle GLED [PF3]
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


int main(void)
{

    SetSystemClock(SYSCTL_SYSDIV_16);//Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= CLK_GPIOF;  //Enable clock for PORTF

    GPIO_PORTF_DIR_R  = PF3;         //Output [PF3]
    GPIO_PORTF_DEN_R  = PF3;         //Enable [PF3]
    Time1A_1sec_delay();             //Timer1 configuration

    while(1)
    {

        // do nothing and wait for the interrupt to occur
    }
}

