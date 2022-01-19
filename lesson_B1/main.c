/*
 * Lesson B1: SYTICK TIMER - Sem interrupção
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



int main()
{
    SetSystemClock(SYSCTL_SYSDIV_64);  //Parameters from sysctl.h (TivaWare Parameter) 3.125MHz
    SYSCTL_RCGCGPIO_R |= CLK_GPIOF;    //Enable clock for PORTF
    GPIO_PORTF_DEN_R  = PF3;           //Enable [PF3]  - GPIO_PORTF_DEN_R  = 0x8
    GPIO_PORTF_DIR_R  = PF3;           //Output [PF3]

    NVIC_ST_RELOAD_R = 156249990;                                //(5 / 1/3.125Mhz) - 5 segundos @3.125Mhz
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_ENABLE;//Enable counter and select system bus clock
    NVIC_ST_CURRENT_R &= NVIC_ST_CURRENT_S;                      //Initialize current value register

    while (1)
    {
        if(NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) //Flag COUNT vai para 1 quando o contador vai para zero
        {
            GPIO_PORTF_DATA_R ^= (PF3);        //Toggle G_LED
      }
    }
}

