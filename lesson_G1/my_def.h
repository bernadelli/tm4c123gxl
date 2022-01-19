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
void I2C3_Init ( void );
static int I2C_wait_till_done(void);
int I2C3_Write_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data);
int I2C3_Read_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data);

#endif /* MY_DEF_H_ */

