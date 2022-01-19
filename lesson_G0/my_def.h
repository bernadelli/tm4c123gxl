/*
 * Lesson Lesson G1: I2C Master [PD0] and [PD1]
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
void I2C0_ini_master();
void I2C0_write(char,char);

#endif /* MY_DEF_H_ */

