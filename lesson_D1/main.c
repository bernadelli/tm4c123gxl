/*
 * Lesson D1: ADC - Toggle GLED [PF3]
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jun/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


int main(void)
{

    unsigned int adc_value;
    SetSystemClock(SYSCTL_SYSDIV_16);//Parameters from sysctl.h (TivaWare Parameter)


    /* Enable Clock to ADC0 and GPIO pins*/
    //13.4.1 Module initialization
    //1 ADC clock
    SYSCTL_RCGCADC_R |= (1<<0);    /* AD0 clock enable*/
    //2 GPIO clock
    SYSCTL_RCGCGPIO_R |= (1<<4);   /* Enable Clock to GPIOE or PE3/AN0 */

    /* initialize PE3 for AIN0 input  */
    //3
    GPIO_PORTE_AFSEL_R |= (1<<3);       /* enable alternate function */
    //4
    GPIO_PORTE_DEN_R &= ~(1<<3);        /* disable digital function */
    //5
    GPIO_PORTE_AMSEL_R |= (1<<3);       /* enable analog function */

    /* initialize sample sequencer3 */
    //13.4.2 Sample sequencer configuration
    //1
    ADC0_ACTSS_R &= ~(1<<3);        /* disable SS3 during configuration */
    //2
    ADC0_EMUX_R &= ~0xF000;    /* software trigger conversion */
    //4
    ADC0_SSMUX3_R = 0;         /* get input from channel 0 */ /*ss3 1 sample 1 fifo*/
    //5
    ADC0_SSCTL3_R |= (1<<1)|(1<<2);        /* take one sample at a time, set flag at 1st sample */
    //7
    ADC0_ACTSS_R |= (1<<3);         /* enable ADC0 sequencer 3 */

    /*Initialize PF3 as a digital output pin */
    SYSCTL_RCGCGPIO_R |= 0x20; // turn on bus clock for GPIOF
    GPIO_PORTF_DIR_R  |= 0x08; //set GREEN pin as a digital output pin
    GPIO_PORTF_DEN_R  |= 0x08;  // Enable PF3 pin as a digital pin

    while(1)
    {

        ADC0_PSSI_R |= (1<<3);        /* Enable SS3 conversion or start sampling data from AN0 */ /*EMUX configuration*/
        while((ADC0_RIS_R & 8) == 0) ;   /* Wait until sample conversion completed*/
            adc_value = ADC0_SSFIFO3_R; /* read ADC conversion result from SS3 FIFO*/
            ADC0_ISC_R = 8;          /* clear conversion clear flag bit*/
            /*control Green PF3->LED */
            if(adc_value >= 2048)
                GPIO_PORTF_DATA_R  = 0x08; /* turn on green LED*/
            else if(adc_value < 2048)
                GPIO_PORTF_DATA_R  = 0x00; /* turn off green LED*/
    }
}

