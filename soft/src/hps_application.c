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
 * Author               : Maillard Patrick
 * Date                 : 26.01.25
 *
 * Context              : Main program
 *
 *****************************************************************************************
 * Brief: Mesure du temps de reaction avec la carte DE1-SoC et MAX10
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0    26.01.25    MaillardP    Finished version
 *
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "axi_lw.h"
#include "uart_function.h"
#include "avalon_function.h"

int __auto_semihosting;

void set_A9_IRQ_stack(void);
void config_GIC(void);
void enable_A9_interrupts(void);

int main(void){

    printf("Laboratoire: Mesure du temps de reaction \n");

    volatile uint32_t *constante = (volatile uint32_t *)H2F_BASE_ADD;
	printf("Constante à l'adresse 0xFF200000 de l'AXI: 0x%X\n", *constante);
	constante = (volatile uint32_t *)INTERFACE_BASE_ADD;
	printf("Constante à l'adresse 0xFF210000 de notre interface: 0x%X\n", *constante);

    // Check if the Max10 is connected
    if(!Max10_check_status()){
        printf("Erreur Max10: Configuration non valide.\n");
		return -1;
    }

    // program initialisation
    Max10_init();
    disable_counter();
    Leds_write(LED_OFF);
    Segs7_init();
    uart_config(); 
    send_to_uart("Reaction Time Measurement Application:\r\n    - Press KEY1 to start a reaction time measurement.\r\n    - Follow the instructions displayed on the Max10_leds.\r\n    - Press KEY0 to stop the measurement.\r\n");

    // IRQ initialisation
    clear_irq();
    set_A9_IRQ_stack();
    config_GIC();
    enable_A9_interrupts();

    uint32_t rand_value;

    while(true){
        // always display based of the switch value
        Seg7_display(Switchs_read());

        if(Key_read_edge(1)){
            no_error(); 
            send_to_uart("Reaction time measurement started.\r\nWait for the start symbol on Max10_leds and press KEY0 as fast as possible.\r\n");
            // Waiting symbol  
            Max10_write_square(WAIT_DISPLAY);    
            new_attemps();

            // generate a new waiting time between 1s and 4s
            rand_value = generate_random();

            // restart the counter
            reset_counter();
            enable_counter();

            // Wait till our counter reach the random value or there is no pression on the key 0 before
            while((counter_current_value() * CLOCK_IN_SECOND <= rand_value) && !new_error());

            // if it there wasnt a pression of key0 before 
            if(!new_error()){
                // so start the measurement
                disable_counter();
                start_game();
                // Beginning symbol
                Max10_write_square(BEGIN_DISPLAY);  

                // restart the counter for the measurement
                reset_counter();
                enable_counter();
            }
        }
    }
}
