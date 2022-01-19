/*
 * Lesson B2: SYTICK TIMER com interrupção
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

    SetSystemClock(SYSCTL_SYSDIV_12_5);  //Parameters from sysctl.h (TivaWare Parameter)
    SYSCTL_RCGCGPIO_R |= CLK_GPIOF;    //Enable clock for PORTF

    /*Initialize [PF3] as a digital output */
    GPIO_PORTF_DIR_R  = PF3;       //Output [PF3]
    GPIO_PORTF_DEN_R  = PF3;       //Enable [PF3]

    NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M -1; //24 bits full
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC +  NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE; //Enable counter and interrupts and select system bus clock
    NVIC_ST_CURRENT_R = NVIC_ST_CURRENT_S; //Initialize current value register

    while(1)
    {

        // do nothing and wait for the interrupt to occur
    }
}

//Função chamada na interrupção. Declarada no tm4c123gh6pm_startup_ccs.c
//linhas 57-58 - Handler added by user
//linha 85 - The vector table
void SysTick_Handler(void)
{
    GPIO_PORTF_DATA_R ^= PF3; //Toggle PF3
}

