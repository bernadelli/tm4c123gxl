/*
 * Lesson I2: uDMA Memory to Peripheral
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version dez/2021  - github.com/bernadelli/pds
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

// Delay function
void delay(unsigned long count) {
   unsigned long i=0;
   for(i=0; i<count; i++);
}
// ************DMA_Init*****************
// Initialize the memory to memory transfer
// This needs to be called once before requesting a transfer
// Inputs:  none
// Outputs: none
#define BIT7 0x00000080
//The aligned variable attribute specifies a minimum alignment for the variable or structure field, measured in bytes.
unsigned int ucControlTable[256] __attribute__ ((aligned(1024)));

void DMA_Init(void){
    int i;
    for(i=0; i<256; i++){
        ucControlTable[i] = 0;
    }

        SYSCTL_RCGCDMA_R |= 0X01;                  // 9.3.1 - 1. µDMA Module Run Mode Clock Gating Control
        UDMA_CFG_R |= 0x01;                        // 9.3.1 - 2. MASTEN Controller Master Enable
        UDMA_CTLBASE_R = (unsigned int)ucControlTable;      // 9.3.1 - 3. Location of the channel control table.
        UDMA_PRIOCLR_R |= BIT7;                   // 9.3.3.1 - 1. default, not high priority
        UDMA_ALTCLR_R |= BIT7;                    // 9.3.3.1 - 2. use primary control
        UDMA_USEBURSTCLR_R |= BIT7;               // 9.3.3.1 - 3. responds to both burst and single requests
        UDMA_REQMASKCLR_R |= BIT7;                // 9.3.4.1 - 4. allow the µDMA controller to recognize requests for this channel

}

// ************DMA_Transfer*****************
// channel 30 is at indices 120,121,122 (primary source,destination,control) and
//               at indices 248,249,250 (alternate source,destination,control not used)
#define CH7 (7*4)

void DMA_Transfer(volatile unsigned int *source, unsigned char *destination, unsigned int count){

    ucControlTable[CH7]   = (unsigned int)source;               // 9.3.3.2 - 1. Source end pointer
    ucControlTable[CH7+1] = (unsigned int)destination+count-1;  // 9.3.3.2 - 2. Destination end pointer
    ucControlTable[CH7+2] = 0x078;                              // 9.3.3.2 - 3. DMA Channel Control Word (DMACHCTL)
    /* DMACHCTL          Bits    Value Description
       DSTINC            31:30   3    Destination address does not increment
       DSTSIZE           29:28   0    8-bit destination data size
       SRCINC            27:26   0    8-bit source address increment
       SRCSIZE           25:24   0    8-bit source data size
       reserved          23:18   0    Reserved
       ARBSIZE           17:14   2    Arbitrates after 4 transfers
       XFERSIZE          13:4    63   Transfer 64 items
       NXTUSEBURST       3       0    N/A for this transfer type
       XFERMODE          2:0     1    Use Basic transfer mode
      */

    UDMA_ENASET_R = BIT7;    // 9.3.3.3 - 1. µDMA Channel 30 is enabled.
    UDMA_SWREQ_R = BIT7;     // 9.3.2.3 - 2. software start,
    // bit 30 in UDMA_ENASET_R become clear when done
    // bits 2:0 ucControlTable[CH30+2] become clear when done
    // vector 62, NVIC interrupt 46, vector address 0x0000.00F8 could be armed or µDMA Software interrupt

}


// ************DMA_Status*****************
// Can be used to check the status of a previous request
// Inputs:  none
// Outputs: true if still active, false if complete
// This routine does not wait for completion
unsigned int DMA_Status(void){
  return (UDMA_ENASET_R & BIT7);  // µDMA Channel 30 enable nit is high if active
}
