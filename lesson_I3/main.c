/*
 * Lesson I2: uDMA Memory to peripheric
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

//---USER FUNCTION'S---//
void SetSystemClock(unsigned int);
void Timer5A_Handler();
void Timer5A_Init(unsigned short);
void DMA_Init(unsigned short);
void DMA_Transfer(volatile unsigned int*, unsigned char*, unsigned int);
unsigned int DMA_Status(void);


//void EnableInterrupts(void);

#define MS 1000
#define SIZE 1024
unsigned char Buffer[SIZE];

void main(void)
{

    unsigned int t=0;
    const int COLORWHEEL[8] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};


    //Parameters from sysctl.h (TivaWare Parameter)
    SetSystemClock(SYSCTL_SYSDIV_2_5);

    SYSCTL_RCGCGPIO_R |= 0x20;    // enable Port F clock
    GPIO_PORTF_LOCK_R = 0x4CF434B;
    GPIO_PORTF_CR_R = 0x01;
    GPIO_PORTF_DIR_R |= 0x0E;                   // make PF3-1 output (PF3-1 built-in LEDs)
    GPIO_PORTF_AFSEL_R &= ~0xFF;                // disable alt funct on PF3-1
    GPIO_PORTF_PUR_R |= 0x11;
    GPIO_PORTF_DEN_R |= 0xFF;                   // enable digital I/O on PF3-1 - configure PF3-1 as GPIO
    GPIO_PORTF_PCTL_R = 0;
    GPIO_PORTF_AMSEL_R = 0;                     // disable analog functionality on PF

    DMA_Init(10*MS);                                 // initialize DMA channel 30 for software transfer
    //Timer5A_Init(64000);
    //EnableInterrupts();
    while(1){
        while(DMA_Status()){

        }
        DMA_Transfer(&GPIO_PORTF_DATA_R, Buffer,SIZE);
        GPIO_PORTF_DATA_R = COLORWHEEL[t&0x07];
        t += 1;
    }
}





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



// ***************** Timer5A_Init ****************
// Activate Timer5A trigger DMA periodically
// Inputs:  period in usec
// Outputs: none
void Timer5A_Init(unsigned short period){
    volatile unsigned int Delay;
    SYSCTL_RCGCTIMER_R |= 0x20;      // 0) activate timer5
    Delay = 0;                       // wait for completion
    TIMER5_CTL_R &= ~0x00000001;     // 1) disable timer5A during setup
    TIMER5_CFG_R = 0x00000004;       // 2) configure for 16-bit timer mode
    TIMER5_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
    TIMER5_TAILR_R = period-1;       // 4) reload value
    TIMER5_TAPR_R = 49;              // 5) 1us timer5A
    TIMER5_ICR_R = 0x00000001;       // 6) clear timer5A timeout flag
    TIMER5_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
    NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|0x00000040; // 8) priority 2
    NVIC_EN2_R = 0x10000000;         // 9) enable interrupt 19 in NVIC
    // vector number 108, interrupt number 92
    TIMER5_CTL_R |= 0x00000001;      // 10) enable timer5A
// interrupts enabled in the main program after all devices initialized
}

unsigned int Count=0;
Timer5A_Handler(){ // interrupts after each block is transferred
    TIMER5_ICR_R = TIMER_ICR_TATOCINT; // acknowledge timer5A timeout
    Count++;
}

// ************DMA_Init*****************
// Initialize the memory to memory transfer
// This needs to be called once before requesting a transfer
// Inputs:  none
// Outputs: none
#define BIT8 0x00000100
//The aligned variable attribute specifies a minimum alignment for the variable or structure field, measured in bytes.
int ucControlTable[256] __attribute__ ((aligned(1024)));

void DMA_Init(unsigned short period){
    int i;
    volatile unsigned int delay;

    for(i=0; i<256; i++){
        ucControlTable[i] = 0;
    }

        SYSCTL_RCGCDMA_R |= 0X01;                  // 9.3.1 - 1. µDMA Module Run Mode Clock Gating Control
        delay = SYSCTL_RCGCDMA_R;
        UDMA_CFG_R |= 0x01;                        // 9.3.1 - 2. MASTEN Controller Master Enable
        UDMA_CTLBASE_R = (unsigned int)ucControlTable;      // 9.3.1 - 3. Location of the channel control table.
        UDMA_CHMAP1_R = (UDMA_CHMAP1_R & 0xFFFFFFF0) | 0x00000003;
        UDMA_PRIOCLR_R |= BIT8;                   // 9.3.2.1 - 1. default, not high priority
        UDMA_ALTCLR_R |= BIT8;                    // 9.3.2.1 - 2. use primary control
        UDMA_USEBURSTCLR_R |= BIT8;               // 9.3.2.1 - 3. responds to both burst and single requests
        UDMA_REQMASKCLR_R |= BIT8;                // 9.3.2.1 - 4. allow the µDMA controller to recognize requests for this channel

        Timer5A_Init(period);

}

// ************DMA_Transfer*****************
// channel 30 is at indices 120,121,122 (primary source,destination,control) and
//               at indices 248,249,250 (alternate source,destination,control not used)
#define CH8 (8*4)

void DMA_Transfer(volatile unsigned int *source, unsigned char *destination, unsigned int count){

    ucControlTable[CH8]   = (unsigned int)source;       // 9.3.2.2 - 1. Source end pointer
    ucControlTable[CH8+1] = (unsigned int)destination+count-1;  // 9.3.2.2 - 2. Destination end pointer
    ucControlTable[CH8+2] = 0x0C000001+((count-1)<<4);   // 9.3.2.2 - 3. DMA Channel Control Word (DMACHCTL)
    /* DMACHCTL          Bits    Value Description
       DSTINC            31:30   0    32-bit destination address increment
       DSTSIZE           29:28   0    32-bit destination data size
       SRCINC            27:26   3    32-bit source address increment
       SRCSIZE           25:24   0    32-bit source data size
       reserved          23:18   0    Reserved
       ARBSIZE           17:14   0    Arbitrates after 8 transfers
       XFERSIZE          13:4  count-1 Transfer count items
       NXTUSEBURST       3       0     N/A for this transfer type
       XFERMODE          2:0     1   Use Auto-request transfer mode
      */

    UDMA_ENASET_R = BIT8;    // 9.3.2.3 - 1. µDMA Channel 30 is enabled.

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
  return (UDMA_ENASET_R & BIT8);  // µDMA Channel 30 enable nit is high if active
}



