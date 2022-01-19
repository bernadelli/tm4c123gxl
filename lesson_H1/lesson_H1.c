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


// Initialization and configuration according data sheet (5.3)
void SetSystemClock(unsigned int SYSDIV2) {
    // 1. Enable USERCC2 in RCC2
    // RCC2[31]=USERCC2 // SYSCTL_RCC2_USERCC2=0x80000000 // RCC2->USERCC2->1
    SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2; // 1=RCC2 override RCC

    // 2. Bypass PLL while initializing. RAW Clock source
    // RCC2[11]=BYPASS2 // SYSCTL_RCC2_BYPASS2=0x00000800 // RCC2->BYPASS2->1
    SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2; // 1=Clock OSC
    // RCC[22]=USESYSDIV // SYSCTL_RCC_USESYSDIV=0x00400000 // RCC->USESYSDIV->0
    // SYSCTL_RCC_R &= (~0x00400000); 0 on reset

    // 3. Set external crystal value and oscillator source for PLL
    // RCC[10:6]=XTAL // SYSCTL_RCC_XTAL_M=0x000007C0 // RCC->XTAL->0
    // SYSCTL_RCC_XTAL_16MHZ=0x00000540 // RCC->XTAL->0x15
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~SYSCTL_RCC_XTAL_M); //Clear RCC[10:6]=XTAL
    SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_16MHZ;      // 16MHz external xtal
    // RCC2[6:4]=OSCSRC2 // SYSCTL_RCC2_OSCSRC2_M=0x00000070 // RCC2->OSCSRC2->0x0
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;//  000=Main Oscillator

    // 4. Clearing PWRDN to active PLL
    // RCC2[13]=PWRDN // SYSCTL_RCC_PWRDN=0x00002000 // RCC2->PWRDN->0x0
    SYSCTL_RCC2_R &= ~SYSCTL_RCC_PWRDN; //0=The PLL operate normally

    // 5. Set system clock
    //Set the USESYSDIV bit in RCC
    // RCC[22]=USESYSDIV // SYSCTL_RCC_USESYSDIV=0x00400000 // RCC->USESYSDIV->1
    // The system clock divider is forced to be used when the PLL is selected as the source.
    SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;  // 1=Divider is the source for the system clock
    // RCC2[30]=DIV400 // SYSCTL_RCC2_DIV400=0x40000000 // RCC2->DIV400->0x1
    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400; //1=Select 400MHz(from PLL)
    // RCC2[28:23]=SYSDIV2 // SYSCTL_RCC2_SYSDIV2_M=0x1F800000 // RCC2->SYSDIV->0x02 // Table 5-6
    // RCC2[22]=SYSDIV2LSB // SYSCTL_RCC2_SYSDIV2LSB=0x00400000 // RCC2->SYSDIV2LSB->0 // Table 5-6
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~SYSCTL_RCC2_SYSDIV2_M);
    SYSCTL_RCC2_R |=SYSDIV2;
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_SYSDIV2LSB;


    // 6. Wait for PLL to lock by polling PLLLRIS
    // RIS[6]=PLLLRIS // SYSCTL_RIS_PLLLRIS=0x00000040 // RIS->PLLLRIS->0x1
    while( (SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0) {};

    // 7. Set BYPASS to 0, select PLL as the source of system clock
    // RCC2[11]=BYPASS2 // SYSCTL_RCC2_BYPASS2=0x00000800 // RCC2->BYPASS2->0x0
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2; // 0=Clock PLL
}



void SPI1_init(void)
{
    //15.4 Initialization and Configuration
    //1. Enable the SSI module
    SYSCTL_RCGCSSI_R |= (1<<1);   /*set clock enabling bit for SPI1 */
    //2. Enable the clock to the appropriate GPIO module
    SYSCTL_RCGCGPIO_R |= (1<<3); /* enable clock to GPIOD for SPI1 */
    SYSCTL_RCGCGPIO_R |= (1<<5); /* enable clock to GPIOF for slave select */

    /*Initialize PD3 and PD0 for SPI1 alternate function*/

    //3. Set alternate function to GPIO pin
    GPIO_PORTD_AFSEL_R |= 0x09;       /* enable alternate function of RD0 and RD3*/
    //4. Assign SSI signals to appropriate pins
    GPIO_PORTD_PCTL_R &= ~0x0000F00F; /* assign RD0 and RD3 pins to SPI1 */
    GPIO_PORTD_PCTL_R |= 0x00002002;  /* assign RD0 and RD3 pins to SPI1  */
    //5A. Enable the pin's digital function
    GPIO_PORTD_AMSEL_R &= ~0x09;      /* disable analog functionality RD0 and RD3 */
    GPIO_PORTD_DEN_R |= 0x09;         /* Set RD0 and RD3 as digital pin */

    /* Initialize PF2 as a digital output as a slave select pin */

    GPIO_PORTF_DEN_R |= (1<<2);         /* set PF2 pin digital */
    GPIO_PORTF_DIR_R |= (1<<2);         /* set PF2 pin output */
    GPIO_PORTF_DATA_R |= (1<<2);        /* keep SS idle high */


    //SSI config
    //1. SSICR1[SSE] clear
    //2. SICR1 0x0000.0000 for master operations
    /* Select SPI1 as a Master, POL = 0, PHA = 0, clock = 4 MHz, 8 bit data */
    SSI1_CR1_R = 0;          /* disable SPI1 and configure it as a Master */
    //3. Configure the SSI clock source
    SSI1_CC_R = 0;           /* Enable System clock Option */
    //4. Configure the clock prescale divisor
    SSI1_CPSR_R = 4;         /* Select prescaler value of 4 .i.e 16MHz/4 = 4MHz */
    //5. Protocol Freescale SPI - SCR - SPH - SPO and DSS
    SSI1_CR0_R = 0x00007;     /* 4MHz SPI1 clock, SPI mode, 8 bit data */
    //6. Optionally
    //7. Enable SSI
    SSI1_CR1_R |= 2;         /* enable SPI1 */
}

/* This function generates delay in ms */
/* calculations are based on 16MHz system clock frequency */

void SPI1_Write(unsigned char data)
{
    GPIO_PORTF_DATA_R &= ~(1<<2);       /* Make PF2 Selection line (SS) low */
    while((SSI1_SR_R & 2) == 0); /* wait untill Tx FIFO is not full */
    SSI1_DR_R = data;            /* transmit byte over SSI1Tx line */
    while(SSI1_SR_R & 0x10);     /* wait until transmit complete */
    GPIO_PORTF_DATA_R |= 0x04;        /* keep selection line (PF2) high in idle condition */
}


void Delay_ms(int time_ms)
{
    int i, j;
    for(i = 0 ; i < time_ms; i++)
        for(j = 0; j < 3180; j++)
            {}  /* excute NOP for 1ms */
}
