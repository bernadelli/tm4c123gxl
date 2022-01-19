/*
 * Lesson G1: I2C Master [PD0] and [PD1]
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
    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)
    char data[2] = {0x01,0x02};
    I2C3_Init();
    SYSCTL_RCGCGPIO_R |= 0x20;   /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlockGPIOCR register
    GPIO_PORTF_PUR_R |= 0x10;        // Enable Pull Up resistor PF4
    GPIO_PORTF_DIR_R |= 0x08;          //set PF1 as an output and PF4 as an input pin
    GPIO_PORTF_DEN_R |= 0x18;         // Enable PF1 and PF4 as a digital GPIO pins

        while (1) {
            unsigned int    value = GPIO_PORTF_DATA_R;
            value = value >> 1;
            GPIO_PORTF_DATA_R = value;
            if(value & (5<<1))
            I2C3_Write_Multiple(4, 0, 1, &data[1]);
            else
            I2C3_Write_Multiple(4, 0, 1, &data[0]);

        }

}

