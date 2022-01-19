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


// I2C initialization and GPIO alternate function configuration
void I2C3_Init ( void )
{
    SYSCTL_RCGCI2C_R   |= 0x08; // Enable the clock for I2C3
    SYSCTL_RCGCGPIO_R |= 0x08;   // Enable system clock to PORTD */
    GPIO_PORTD_DEN_R |= 0x03; // Assert DEN for port D
    // Configure Port D pins 0 and 1 as I2C 3
    GPIO_PORTD_AFSEL_R |= 0x03;
    GPIO_PORTD_ODR_R |= 0x02; // SDA (PD1 ) pin as open drain
    GPIO_PORTD_PCTL_R |= 0x33;
    I2C3_MCR_R  = 0x0010 ; // Enable I2C3 master function
    /* Configure I2C 3 clock frequency (1 + TIME_PERIOD ) = (SYS_CLK /(2* (SCL_LP + SCL_HP) * I2C_CLK_Freq))
      TIME_PERIOD = (16,000,000/(2(6+4) *100000)) - 1 = 7 */
    I2C3_MTPR_R = 0x07 ;
}


/* wait until I2C Master module is busy */
/* and if not busy and no error return 0 */
static int I2C_wait_till_done(void)
{
    while(I2C3_MCS_R & 1);   /* wait until I2C master is not busy */
    return I2C3_MCS_R & 0xE; /* return I2C error code, 0 if no error*/
}


// Receive one byte of data from I2C slave device
int I2C3_Write_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data)
{
    char error;
    if (bytes_count <= 0)
        return -1;                  /* no write was performed */
    /* send slave address and starting address */
    I2C3_MSA_R = slave_address << 1;
    I2C3_MDR_R = slave_memory_address;
    I2C3_MCS_R = 3;                  /* S-(saddr+w)-ACK-maddr-ACK */

    error = I2C_wait_till_done();   /* wait until write is complete */
    if (error) return error;

    /* send data one byte at a time */
    while (bytes_count > 1)
    {
        I2C3_MDR_R = *data++;             /* write the next byte */
        I2C3_MCS_R = 1;                   /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        bytes_count--;
    }

    /* send last byte and a STOP */
    I2C3_MDR_R = *data++;                 /* write the last byte */
    I2C3_MCS_R  = 5;                       /* -data-ACK-P */
    error = I2C_wait_till_done();
    while(I2C3_MCS_R  & 0x40);             /* wait until bus is not busy */
    if (error) return error;
    return 0;       /* no error */
}


/* This function reds from slave memory location of slave address */
/* read address should be specified in the second argument */
/* read: S-(saddr+w)-ACK-maddr-ACK-R-(saddr+r)-ACK-data-ACK-data-ACK-...-data-NACK-P */
int I2C3_Read_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data)
{
    char error;

    if (bytes_count <= 0)
        return -1;         /* no read was performed */

    /* send slave address and starting address */
    I2C3_MSA_R = slave_address << 1;
    I2C3_MDR_R = slave_memory_address;
    I2C3_MCS_R  = 3;       /* S-(saddr+w)-ACK-maddr-ACK */
    error = I2C_wait_till_done();
    if (error)
        return error;

    /* to change bus from write to read, send restart with slave addr */
    I2C3_MSA_R = (slave_address << 1) + 1;   /* restart: -R-(saddr+r)-ACK */

    if (bytes_count == 1)             /* if last byte, don't ack */
        I2C3_MCS_R  = 7;              /* -data-NACK-P */
    else                            /* else ack */
        I2C3_MCS_R  = 0xB;            /* -data-ACK- */
    error = I2C_wait_till_done();
    if (error) return error;

    *data++ = I2C3_MDR_R;            /* store the data received */

    if (--bytes_count == 0)           /* if single byte read, done */
    {
        while(I2C3_MCS_R  & 0x40);    /* wait until bus is not busy */
        return 0;       /* no error */
    }

    /* read the rest of the bytes */
    while (bytes_count > 1)
    {
        I2C3_MCS_R  = 9;              /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        bytes_count--;
        *data++ = I2C3_MDR_R;        /* store data received */
    }

    I2C3_MCS_R = 5;                  /* -data-NACK-P */
    error = I2C_wait_till_done();
    *data = I2C3_MDR_R;              /* store data received */
    while(I2C3_MCS_R  & 0x40);        /* wait until bus is not busy */

    return 0;       /* no error */
}
