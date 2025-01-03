/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : uart_function.c
 * Author               : 
 * Date                 : 
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: 
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
#include "uart_function.h"

void uart_config(){

    uint32_t *uart_lcr  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + LCR));
    uint32_t *uart_fcr  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + FCR));
    uint32_t *uart_dll  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + RBR_THR_DLL));
    uint32_t *uart_dlh  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + IER_DLH));
    uint32_t *uart_ier  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + IER_DLH));

    //configure DLAB of lcr
    *uart_lcr |= 0x80;

    // divisor = l4_sp_clk/ (16*baudrate) =~ 651.04
    // Configurer le baud rate (651 = 0x28B)
    *uart_dll = 0x8B; // LSB
    *uart_dlh = 0x02; // MSB

    // Configurer LCR : 8 bits de données, parité désactivée, 1 bit de stop (a vérifier)
    *uart_lcr = 0x03;

    // Activer les FIFO en réception et transmission (a vérifier)
    *uart_fcr = 0x01;

    // clear DLAB of lcr
    *uart_ier = 0x00;
}

void uart_send_char(char c) {

    uint32_t *uart_thr  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + RBR_THR_DLL)); 
    uint32_t *uart_lsr  = ((volatile unsigned int *)(UART0_BASE_ADDRESS + LSR)); 

    while (!(*uart_lsr & LSR_THRE_MASK));

    *uart_thr = c;
}

void send_to_uart(const char* mystring){
    while (*mystring) {
        uart_send_char(*mystring); 
        mystring++;
    }
}



