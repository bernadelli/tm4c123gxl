/*
 * my_def.h Part of Lesson A8: Push Button toggle
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version jan/2021  - github.com/bernadelli/pds
 */

#ifndef MY_DEF_H_
#define MY_DEF_H_

//---PORT-F I/O---//
#define PF1 0x02
#define PF2 0x04
#define PF3 0x08
#define PF4 0x10

//---USER FUNCTION'S---//
void delay(unsigned long);
void SetSystemClock(unsigned int);

#endif /* MY_DEF_H_ */

