/*
 * Lesson H1: SPI SSI1 - Transmits character A and B with a delay of one second
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version set/2021  - github.com/bernadelli/pds
 */

#ifndef MY_DEF_H_
#define MY_DEF_H_

//---PORT-F I/O---//


//---USER FUNCTION'S---//
void SetSystemClock(unsigned int);
/* function prototype of SPI and Delay */
void SPI1_init(void);
void SPI1_Write(unsigned char data);
void Delay_ms(int time_ms);
#endif /* MY_DEF_H_ */

