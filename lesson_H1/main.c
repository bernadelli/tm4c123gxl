
/*
 * Lesson H1: SPI SSI1 - Transmits character A and B with a delay of one second
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version nov/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


int main(void)
{
    unsigned char val1 = 'A';
    unsigned char val2 = 'B';

    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)
    SPI1_init();

    while(1){
        SPI1_Write(val1); /* write a character */
        Delay_ms(1000);
        SPI1_Write(val2); /* write a character */
        Delay_ms(1000);
    }
}

