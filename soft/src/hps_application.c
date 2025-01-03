/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : hps_application.c
 * Author               : 
 * Date                 : 
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: Mesure du temps de reaction avec la carte DE1-SoC et MAX10
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 
 *
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "axi_lw.h"
#include "uart_function.h"

int __auto_semihosting;

/****************************************************************************************
 * Uart 0 interrupt number is 194 (p.682)
 * Uart0 base adress is 0xFFC02000 (p.3073)
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 ****************************************************************************************/

int main(void){

    printf("Laboratoire: Mesure du temps de reaction \n");
    
    uart_config();

    while (true){
        send_to_uart("Test\n");
        for (volatile int i = 0; i < 1000000; i++);
    }

}
