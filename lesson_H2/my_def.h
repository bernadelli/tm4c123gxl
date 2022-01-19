/*
 * Lesson H2: SPI SSI1 - Hello
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version nov/2021  - github.com/bernadelli/pds
 */

#ifndef MY_DEF_H_
#define MY_DEF_H_

//---PORT-F I/O---//


//---USER FUNCTION'S---//
void SetSystemClock(unsigned int);
void spi_master_ini(void);
void send_byte(char);
void send_str(char*);

#endif /* MY_DEF_H_ */

