/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : avalon_function.h
 * Author               : Rafael Dousse & Patrick Maillard
 * Date                 : 09.11.2024
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: Avalon functions
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 0.0    09.11.2024  RAD & PAM     Initial version.
 *
*****************************************************************************************/
#include "avalon_function.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

const unsigned char seven_segment_map[16] = {
        0b1000000,  // 0
        0b1111001,  // 1
        0b0100100,  // 2
        0b0110000,  // 3
        0b0011001,  // 4
        0b0010010,  // 5
        0b0000010,  // 6
        0b1111000,  // 7
        0b0000000,  // 8
        0b0010000,  // 9
		0b0001000,	// A
		0b0000011,	// b
		0b1000110, 	// C
		0b0100001,	// d
		0b0000110,	// E
		0b0001110	// F
};

double my_reaction_time = 0;
double best_reaction_time = 9.999;
double worst_reaction_time = 0;
uint32_t nbr_of_error = 0;
uint32_t nbr_of_attempt = 0;

bool game_started = false;
bool error = false;

uint32_t generate_random(void){
    uint32_t random_value;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("Erreur ouverture /dev/urandom");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &random_value, sizeof(random_value)) != sizeof(random_value)) {
        perror("Erreur lecture /dev/urandom");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
	return (random_value % 5);
}

uint32_t Switchs_read(void)
{
	volatile uint32_t *value_switchs = SWITCH_REG;
	return ((*value_switchs & SWITCHS_BITS));
}

//TODO
// Segs7_init function : Initialize all 7-segments display in PIO core (HEX3 to HEX0)
void Segs7_init(void){
	Seg7_write_hex(0, 0);
	Seg7_write_hex(1, 0);
	Seg7_write_hex(2, 0);
	Seg7_write_hex(3, 0);
}

// Seg7_write function : Write digit segment value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= digit segment value to be applied on the selected 7-segments (maximum 0x7F to switch ON all segments)
// Return : None
void Seg7_write(int seg7_number, uint32_t value){

	uint32_t hex_mask;
	switch(seg7_number){
		case 0:
			hex_mask = HEX0_BITS;
			break;
		case 1:
			hex_mask = HEX1_BITS;
			value = value << 7;
			break;
		case 2:
			hex_mask = HEX2_BITS;
			value = value << 14;
			break;
		case 3:
			hex_mask = HEX3_BITS;
			value = value << 21;
			break;
		default:
			printf("7-segment chosen does not exist or shouldn't be used");
			return;
	}
	volatile uint32_t *value_hex = INTERFACE_REG(HEX_OFFSET);
	*value_hex &= ~hex_mask;
	*value_hex |= (~value & hex_mask);
}

// Seg7_write function : Write digit segment value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= digit segment value to be applied on the selected 7-segments (maximum 0x7F to switch ON all segments)
// Return : None
void Seg7_reaction_time(double reaction_time){
	uint32_t hex0_to_3;
	uint8_t hex0, hex1, hex2, hex3;
	int mynumber = (double)reaction_time * 1000;
	if (mynumber > 9999) {
		mynumber = 9999;
	}
	hex0 = mynumber % 10;
	hex1 = (mynumber / 10) % 10;
	hex2 = (mynumber / 100) % 10;
	hex3 = (mynumber / 1000) % 10;

	hex0_to_3 = (seven_segment_map[hex3] << 21) | (seven_segment_map[hex2] << 14) | (seven_segment_map[hex1] << 7) | seven_segment_map[hex0];
	volatile uint32_t *value_hex = INTERFACE_REG(HEX_OFFSET);
	*value_hex &= 0xFFFFFFFF;
	*value_hex = (hex0_to_3);
}

// Seg7_write function : Write digit segment value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= digit segment value to be applied on the selected 7-segments (maximum 0x7F to switch ON all segments)
// Return : None
void Seg7_write_all(uint32_t number){
	uint32_t hex0_to_3;
	uint8_t hex0, hex1, hex2, hex3;
	if (number > 9999) {
		number = 9999;
	}
	hex0 = number % 10;
	hex1 = (number / 10) % 10;
	hex2 = (number / 100) % 10;
	hex3 = (number / 1000) % 10;

	hex0_to_3 = (seven_segment_map[hex3] << 21) | (seven_segment_map[hex2] << 14) | (seven_segment_map[hex1] << 7) | seven_segment_map[hex0];
	volatile uint32_t *value_hex = INTERFACE_REG(HEX_OFFSET);
	*value_hex &= 0xFFFFFFFF;
	*value_hex = (hex0_to_3);
}


// TODO
// Seg7_write_hex function : Write an Hexadecimal value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= Hexadecimal value to be display on the selected 7-segments, form 0x0 to 0xF
// Return : None
void Seg7_display(uint32_t switch_value){
	if(switch_value & SWITCH0_MASK){
		Seg7_reaction_time(best_reaction_time);
	}
	else if(switch_value & SWITCH1_MASK){
		Seg7_reaction_time(worst_reaction_time);
	}
	else if(switch_value & SWITCH2_MASK){
		Seg7_write_all(nbr_of_error);
	}
	else if(switch_value & SWITCH3_MASK){
		Seg7_write_all(nbr_of_attempt);
	}
	else{
		Seg7_reaction_time(my_reaction_time);
	}
}

// TODO
// Seg7_write_hex function : Write an Hexadecimal value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= Hexadecimal value to be display on the selected 7-segments, form 0x0 to 0xF
// Return : None
void Seg7_write_hex(int seg7_number, uint32_t value){
	if (value > 0xF){
		printf("Value chosen isn't an hexadecimal value");
		return;
	}
	uint32_t hex_mask;
	uint32_t shift = 0;
		switch(seg7_number){
			case 0:
				hex_mask = HEX0_BITS;
				break;
			case 1:
				hex_mask = HEX1_BITS;
				shift = 7;
				break;
			case 2:
				hex_mask = HEX2_BITS;
				shift = 14;
				break;
			case 3:
				hex_mask = HEX3_BITS;
				shift= 21;
				break;
			default:
				printf("7-segment chosen does not exist or shouldn't be used");
				return;
		}
		volatile uint32_t *value_hex = INTERFACE_REG(HEX_OFFSET);
		*value_hex &= ~hex_mask;
		*value_hex |= (seven_segment_map[value] << shift) & hex_mask;
}

void Leds_write(uint32_t value)
{
	if (value > 0x3FF)
	{
	    printf("Error: value is too big\n");
	    return;
	}
	volatile uint32_t *value_leds = LED_REG;
	*value_leds = value; // Remplace directement la valeur des LEDs
}

void Leds_set(uint32_t maskleds)
{
	if (maskleds > 0x3ff)
	{
		printf("Error: maskleds is too big\n");
		return;
	}
	volatile uint32_t *value_leds = LED_REG;
	 *value_leds |= maskleds; // Allume les LEDs spécifiées par le masque
}

uint32_t Leds_read(uint32_t led)
{
	volatile uint32_t *value_leds = LED_REG;

	return (*value_leds & led);
}

void Leds_toggle(uint32_t maskleds){
	volatile uint32_t *value_leds = LED_REG;
	*value_leds ^= maskleds & LEDS_BITS;
}

bool Key_read(int key_number)
{
	volatile uint32_t *dataValue = BOUTON_REG;
	uint32_t my_key_value = (*dataValue >> key_number) & 0x1;
	return !my_key_value;
}

// Variable to store the state of the keys
bool positive_edge[4] = {false, false, false, false};
bool Key_read_edge(int key_number)
{
	volatile uint32_t *dataValue = BOUTON_REG;
	// Get the key number and check its value
	uint32_t my_key_value = (*dataValue >> key_number) & 0x1;

	if (my_key_value == 0 && !positive_edge[key_number])
	{									  // Key is pressed (active low) and no edge detected yet
		positive_edge[key_number] = true; // Mark the positive edge (key press) as detected
		return true; // Rising edge (key press) detected
	}
	else if (my_key_value == 1 && positive_edge[key_number])
	{									   // Key is released (active low)
		positive_edge[key_number] = false; // Reset the edge detection for the next press
	}

	return false;
}

void Max10_init(void){
	uint32_t code = 0x0;
	uint32_t value = 0x0;
	volatile uint32_t *max10_data_reg = SERIAL_LINK_DATA_REG;

	while(code <= 0xA){
		if(serial_transmitter_ready()){
			uint32_t myserial = code << SERIAL_LINK_CODE_SHIFT | value << SERIAL_LINK_DATA_SHIFT;
    		*max10_data_reg = myserial;
			code++;
		}
	}
	
	uint32_t myserial = code << SERIAL_LINK_CODE_SHIFT | value << SERIAL_LINK_DATA_SHIFT;
    *max10_data_reg = myserial;
}


bool Max10_check_status(void)
{
    volatile uint32_t *status = STATUS_REG;
    return ((*status & STATUS_VALID) == STATUS_VALID);
}

void Max10_write_serial_link(uint32_t value, uint8_t code)
{
	volatile uint32_t *max10_data_reg = SERIAL_LINK_DATA_REG;
    if (code > 0x0A)
    {
        printf("Erreur : Valeur de sel invalide.\n");
        return;
    }
	while(!serial_transmitter_ready()){}
	uint32_t myserial = code << SERIAL_LINK_CODE_SHIFT | value << SERIAL_LINK_DATA_SHIFT;
    *max10_data_reg = myserial;
}

void Max10_write_square(uint32_t value){
	uint32_t value_MSB = value >> 16;
	uint32_t value_LSB = value & 0xFFFF;	
	Max10_write_serial_link(value_MSB, 0x7);
	Max10_write_serial_link(value_LSB, 0x6);
}

bool serial_transmitter_ready(){
	volatile uint32_t *ready = SERIAL_LINK_READY_REG;
	uint32_t reg_val = *ready;
    return ((reg_val & SERIAL_LINK_READY_MASK) == SERIAL_LINK_READY_MASK);
}

void enable_counter(){
	volatile uint32_t *enable = ENABLE_COUNTER_REG;
	*enable = ENABLE_COUNTER_MASK;
}

void disable_counter(){
	volatile uint32_t *disable = ENABLE_COUNTER_REG;
	*disable &= ~ENABLE_COUNTER_MASK;
}

bool is_counter_enabled(){
	volatile uint32_t *enable = ENABLE_COUNTER_REG;
	return ((*enable & ENABLE_COUNTER_MASK) == ENABLE_COUNTER_MASK);
}

uint32_t counter_current_value(){
	volatile uint32_t *value = COUNTER_CURRENT_VALUE_REG;
	uint32_t counter_value = *value;
	return counter_value;
}

void reset_counter(){
	volatile uint32_t *reset = RESET_COUNTER_REG;
	*reset |= RESET_COUNTER_MASK;
	*reset &= ~RESET_COUNTER_MASK;
}

void clear_irq(){
	volatile uint32_t *clear = CLEAR_IRQ_REG;
	*clear |= IRQ_CLEAR_MASK;
}

void new_time(){
	if(game_started){
		my_reaction_time = (double)counter_current_value() * 0.000000020;
		if(best_reaction_time > my_reaction_time){
			best_reaction_time = my_reaction_time;
		}
		if(worst_reaction_time < my_reaction_time){
			worst_reaction_time = my_reaction_time;
		}
		// display symbole de fin
		Max10_write_square(END_DISPLAY);  
	}
	else{
		send_to_uart("Error: Premature key press detected.\r\nMeasurement aborted.\r\n");
		nbr_of_error++;
		error = true;
		// display off
		Max10_write_square(0x0);  
	}
	char buffer[256];  // Create a buffer to hold the formatted string
    sprintf(buffer,
            "Reaction time: %f s\r\n"
            "Best time: %f s\r\n"
            "Worst time: %f s\r\n"
            "Errors: %d\r\n"
            "Total attempts: %d\r\n",
            my_reaction_time, best_reaction_time, worst_reaction_time, nbr_of_error, nbr_of_attempt);
	send_to_uart(buffer);

	disable_counter();
	game_started = false;

}

void start_game(){
	game_started = true;
}

void new_attemps(){
	nbr_of_attempt++;
}

bool new_error(){
	return error;
}

void no_error(){
	error = false;
}
