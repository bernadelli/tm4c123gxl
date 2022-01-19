/*
 * Lesson H2: SPI SSI1 - Hello
 * Prof. Dr. Cláriton Bernadelli
 * Electrical engineering - Federal University of Triângulo Mineiro – UFTM
 * Signal processing group - GPS - UFTM - Brazil
 * Version nov/2021  - github.com/bernadelli/pds
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <my_def.h>
#include <stdbool.h>
#include "sysctl.h"


int main(){

    SetSystemClock(SYSCTL_SYSDIV_12_5);//Parameters from sysctl.h (TivaWare Parameter)
    spi_master_ini(); //initializing controller as master
    send_str("Hello SPI \ncommunication");//sending string

}

