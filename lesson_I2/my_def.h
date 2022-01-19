/*
Lesson I1: uDMA Memory to memory
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version dez/2021  - github.com/bernadelli/pds
 */

#ifndef MY_DEF_H_
#define MY_DEF_H_

//---PORT-F I/O---//
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08


//---USER FUNCTION'S---//
void SetSystemClock(unsigned int);
void delay(unsigned long);
void DMA_Init(void);
void DMA_Transfer(volatile unsigned int*, unsigned char*, unsigned int);
unsigned int DMA_Status(void);

#endif /* MY_DEF_H_ */

