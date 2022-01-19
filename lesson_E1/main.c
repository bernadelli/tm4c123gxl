/*
 * Lesson E1: UART - [PE5] as TX5
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jul/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


int main(void)
{
    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)
    /* Initialization and configuration*/
    //14.4 Module initialization
    //1a UART module
    SYSCTL_RCGCUART_R |= 0x20;  /* enable clock to UART5 */
    //2a Enable clock
    SYSCTL_RCGCGPIO_R |= 0x10;  /* enable clock to PORTE for PE4/Rx and RE5/Tx */
    delay(1);
    /* UART0 initialization */
    //1b Disable UART
    UART5_CTL_R = 0;         /* UART5 module disable */
    //2b Integer portion BRD
    UART5_IBRD_R = 104;      /* for 9600 baud rate, integer = 104 */
    //3b Fractional portion BRD
    UART5_FBRD_R = 11;       /* for 9600 baud rate, fractional = 11*/
    //4b Serial parameters
    UART5_LCRH_R = 0x60;     /* data length 8-bit, not parity bit, no FIFO */
    //5b Clock
    UART5_CC_R = 0;          /*select system clock*/
    //6b Optionally - uDMA
    //7b Enable UART
    UART5_CTL_R = 0x301;     /* Enable UART5 module, Rx and Tx */

    /* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
    GPIO_PORTE_DEN_R = 0x30;      /* set PE4 and PE5 as digital */
    //3a Alternate function
    GPIO_PORTE_AFSEL_R = 0x30;    /* Use PE4,PE5 alternate function */
    GPIO_PORTE_AMSEL_R = 0;    /* Turn off analog function*/
    //4a Default
    //5a PMCn
    GPIO_PORTE_PCTL_R = 0x00110000;     /* configure PE4 and PE5 for UART */

    delay(1);

    while(1)
    {
        UART5_Transmitter('B');
        UART5_Transmitter('E');
        UART5_Transmitter('R');
        UART5_Transmitter('N');
        UART5_Transmitter('A');
        UART5_Transmitter('\n');
    }
}

