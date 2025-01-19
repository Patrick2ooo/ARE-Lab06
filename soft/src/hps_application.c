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
#include "avalon_function.h"

int __auto_semihosting;

void set_A9_IRQ_stack(void);
void config_GIC(void);
void enable_A9_interrupts(void);

/****************************************************************************************
 * Uart 0 interrupt number is 194 (p.682)
 * Uart 0 base adress is 0xFFC02000 (p.3073)
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

    volatile uint32_t *constante = (volatile uint32_t *)H2F_BASE_ADD;
	printf("Constante à l'adresse 0xFF200000 de l'AXI: 0x%X\n", *constante);
	constante = (volatile uint32_t *)INTERFACE_BASE_ADD;
	printf("Constante à l'adresse 0xFF210000 de notre interface: 0x%X\n", *constante);

    if(!Max10_check_status()){
        printf("Erreur Max10: Configuration non valide.\n");
		return -1;
    }

    Max10_init();

    Leds_write(LED_OFF);
    Segs7_init();
    uart_config(); 
    send_to_uart("Reaction Time Measurement Application:\r\n    - Press KEY1 to start a reaction time measurement.\r\n    - Follow the instructions displayed on the Max10_leds.\r\n    - Press KEY0 to stop the measurement.\r\n");

    clear_irq();
    set_A9_IRQ_stack();
    config_GIC();
    enable_A9_interrupts();

    uint32_t rand_value;
    uint32_t switch_value;

    while(true){
        Seg7_display(Switchs_read());

        if(Key_read_edge(1)){
            no_error();
            send_to_uart("Reaction time measurement started.\r\nWait for the start symbol on Max10_leds and press KEY0 as fast as possible.\r\n");
            //symbole d'attente    
            Max10_write_square(WAIT_DISPLAY);    
            new_attemps();
            rand_value = generate_random();
            reset_counter();
            enable_counter();
            while((counter_current_value() * 0.000000020 <= rand_value) && !new_error());
            if(!new_error()){
                disable_counter();
                start_game();
                //symbole de début
                Max10_write_square(BEGIN_DISPLAY);  
                reset_counter();
                enable_counter();
            }
        }
    }
    /*
    uint32_t my_reaction_time;
uint32_t best_reaction_time;
uint32_t worst_reaction_time;
uint32_t nbr_of_error;
uint32_t nbr_of_attempt;
*/

    /*reset_counter();
    enable_counter();
    printf("my counter value before = %d\n", counter_current_value());

    for(int i = 0; i < 10; i++){
    	printf("%d\n", i);
    }

    printf("my counter value after = %d\n", counter_current_value());
    disable_counter();*/

    /*uart_config();    
    while (true){
        send_to_uart("Test\r\n");
        for (volatile int i = 0; i < 1000000; i++);
    }*/

    

}
