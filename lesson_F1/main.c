/*
 * Lesson F1:
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version set/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


int main(void)
    {

    int duty_cycle = 4999;

    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)

    /* Clock setting for PWM and GPIO PORT */
    SYSCTL_RCGCPWM_R |= 2;       /* Enable clock to PWM1 module */
    SYSCTL_RCGCGPIO_R |= 0x20;  /* Enable system clock to PORTF */
    SYSCTL_RCC_R |= (1<<20);    /* Enable System Clock Divisor function  */
    SYSCTL_RCC_R |= 0x000E0000; /* Use pre-divider value of 64 and after that feed clock to PWM1 module*/

    /* Setting of PF2 pin for M1PWM6 channel output pin */
    GPIO_PORTF_AFSEL_R |= (1<<2);  /* PF2 sets a alternate function */
    GPIO_PORTF_PCTL_R &= ~0x00000F00; /*set PF2 as output pin */
    GPIO_PORTF_PCTL_R |= 0x00000500; /* make PF2 PWM output pin */
    GPIO_PORTF_DEN_R |= (1<<2);      /* set PF2 as a digital pin */

    PWM1_3_CTL_R &= ~(1<<0);   /* Disable Generator 3 counter */
    PWM1_3_CTL_R &= ~(1<<1);   /* select down count mode of counter 3*/
    PWM1_3_GENA_R = 0x0000008C;  /* Set PWM output when counter reloaded and clear when matches PWMCMPA */
    PWM1_3_LOAD_R = 5000;     /* set load value for 50Hz 16MHz/65 = 250kHz and (250KHz/5000) */
    PWM1_3_CMPA_R = 4999;     /* set duty cyle to to minumum value*/
    PWM1_3_CTL_R = 1;           /* Enable Generator 3 counter */
    PWM1_ENABLE_R = 0x40;      /* Enable PWM1 channel 6 output */

    while(1)
        {
            duty_cycle = duty_cycle - 10;
            if (duty_cycle <= 0)
                duty_cycle = 5000;
            PWM1_3_CMPA_R = duty_cycle;
            delay_ms(100);
    }
}

