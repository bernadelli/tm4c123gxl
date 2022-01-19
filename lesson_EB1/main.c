/*
 * Lesson EB1: UART - [PE5] as TX5 - [PE4] as RX4 and interrupt
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
    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)

    SYSCTL_RCGCUART_R |= 0x20;  /* enable clock to UART5 */
    SYSCTL_RCGCGPIO_R |= 0x10;  /* enable clock to PORTE for PE4/Rx and RE5/Tx */
    delay(1);

    /* UART0 initialization */
    UART5_CTL_R = 0;         /* UART5 module disbable */
    UART5_IBRD_R = 104;      /* for 9600 baud rate, integer = 104 */
    UART5_FBRD_R = 11;       /* for 9600 baud rate, fractional = 11*/
    UART5_CC_R = 0;          /*select system clock*/
    UART5_LCRH_R = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
    UART5_CTL_R = 0x301;     /* Enable UART5 module, Rx and Tx */

    /* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
    GPIO_PORTE_DEN_R = 0x30;      /* set PE4 and PE5 as digital */
    GPIO_PORTE_AFSEL_R = 0x30;    /* Use PE4,PE5 alternate function */
    GPIO_PORTE_AMSEL_R = 0;  /* Turn off analg function*/
    GPIO_PORTE_PCTL_R  = 0x00110000;     /* configure PE4 and PE5 for UART */

    // enable interrupt
    UART5_ICR_R &= ~(0x0780);; // Clear receive interrupt
    UART5_IM_R  = 0x0010;
    NVIC_EN1_R |= (1<<29);//NVIC->ISER[1] |= 0x20000000; /* enable IRQ61 for UART0 */

    SYSCTL_RCGCGPIO_R |= 0x20; // turn on bus clock for GPIOF
    GPIO_PORTF_DIR_R  |= 0x02; //set GREEN pin as a digital output pin
    GPIO_PORTF_DEN_R  |= 0x02;  // Enable PF3 pin as a digital pin

    delay(1);
    printstring("Enter the command\n");
    delay(10);

    while(1)
    {

    }
}

void UART5_Handler(void)
    {
    unsigned char rx_data = 0;
    UART5_ICR_R &= ~(0x010); // Clear receive interrupt
    rx_data = UART5_DR_R ; // get the received data byte
        if(rx_data == 'A')
        GPIO_PORTF_DATA_R  = 0x02;
        else if(rx_data == 'B')
        GPIO_PORTF_DATA_R = 0x00;
    UART5_Transmitter(rx_data); // send data that is received
}
