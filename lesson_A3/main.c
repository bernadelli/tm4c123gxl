/*
 * Lesson A3: Clock setting and blink the RGB led
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jan/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

//---PORT-F I/O---//
#define PF1 0x02
#define PF2 0x04
#define PF3 0x08

//---USER CONSTANTS---//
#define time_delay 1000000

//---USER FUNCTION'S---//
void delay(unsigned long);
void SetSystemClock_80MHz();

void main(void)
{
    SetSystemClock_80MHz(); // Set 80MHz clock
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for PORTF
    GPIO_PORTF_DEN_R  = 0x0E;  // Enable PORTF [PF3:PF1] as digital pins
    GPIO_PORTF_DIR_R  = 0x0E;  // Configure PORTF [PF3:PF1] as digital output pins

    while (1) {

        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF1; // turn on LED_R
        delay(time_delay);
        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF2; // turn on LED_B
        delay(time_delay);
        GPIO_PORTF_DATA_R = 0;
        GPIO_PORTF_DATA_R |= PF3; // turn on LED_G
        delay(time_delay);
    }
}


void delay(unsigned long count) {
   unsigned long i=0;
   for(i=0; i<count; i++);
}

// Initialization and configuration according data sheet (5.3)
void SetSystemClock_80MHz() {
    // 1. Enable USERCC2 in RCC2
    // RCC2[31]=USERCC2 // SYSCTL_RCC2_USERCC2=0x80000000 // RCC2->USERCC2->1
    SYSCTL_RCC2_R |= 0x80000000; // 1=RCC2 override RCC

    // 2. Bypass PLL while initializing. RAW Clock source
    // RCC2[11]=BYPASS2 // SYSCTL_RCC2_BYPASS2=0x00000800 // RCC2->BYPASS2->1
    SYSCTL_RCC2_R |= 0x00000800; // 1=Clock OSC
    // RCC[22]=USESYSDIV // SYSCTL_RCC_USESYSDIV=0x00400000 // RCC->USESYSDIV->0
    // SYSCTL_RCC_R &= (~0x00400000); 0 on reset

    // 3. Set external crystal value and oscillator source for PLL
    // RCC[10:6]=XTAL // SYSCTL_RCC_XTAL_M=0x000007C0 // RCC->XTAL->0
    // SYSCTL_RCC_XTAL_16MHZ=0x00000540 // RCC->XTAL->0x15
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) //Clear RCC[10:6]=XTAL
                  + (0x15 << 6);// 16MHz external xtal
    // RCC2[6:4]=OSCSRC2 // SYSCTL_RCC_OSCSRC_MAIN=0x00000000 // RCC2->OSCSRC2->0x0
    SYSCTL_RCC2_R &= ~0x00000070;//  000=Main Oscillator

    // 4. Clearing PWRDN to active PLL
    // RCC2[13]=PWRDN // SYSCTL_RCC_PWRDN=0x00002000 // RCC2->PWRDN->0x0
    SYSCTL_RCC2_R &= ~0x00002000; //0=The PLL operate normally

    // 5. Set system clock
    //Set the USESYSDIV bit in RCC
    // RCC[22]=USESYSDIV // SYSCTL_RCC_USESYSDIV=0x00400000 // RCC->USESYSDIV->1
    // The system clock divider is forced to be used when the PLL is selected as the source.
    SYSCTL_RCC_R |= 0x00400000;  // 1=Divider is the source for the system clock
    // RCC2[30]=DIV400 // SYSCTL_RCC2_DIV400=0x40000000 // RCC2->DIV400->0x1
    SYSCTL_RCC2_R |= 0x40000000; //1=Select 400MHz(from PLL)
    // RCC2[28:23]=SYSDIV2 // SYSCTL_RCC2_SYSDIV2_M=0x1F800000 // RCC2->SYSDIV->0x02 // Table 5-6
    // RCC2[22]=SYSDIV2LSB // SYSCTL_RCC2_SYSDIV2LSB=0x00400000 // RCC2->SYSDIV2LSB->0 // Table 5-6
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (0x04 << 22);


    // 6. Wait for PLL to lock by polling PLLLRIS
    // RIS[6]=PLLLRIS // SYSCTL_RIS_PLLLRIS=0x00000040 // RIS->PLLLRIS->0x1
    while( (SYSCTL_RIS_R & 0x00000040) == 0) {};

    // 7. Set BYPASS to 0, select PLL as the source of system clock
    // RCC2[11]=BYPASS2 // SYSCTL_RCC2_BYPASS2=0x00000800 // RCC2->BYPASS2->0x0
    SYSCTL_RCC2_R &= ~0x00000800; // 0=Clock PLL
}

