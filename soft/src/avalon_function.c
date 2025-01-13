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

uint32_t Switchs_read(void)
{
	volatile uint32_t *value_switchs = SWITCH_REG;
	return ((*value_switchs & SWITCHS_BITS));
}

//TODO
// Segs7_init function : Initialize all 7-segments display in PIO core (HEX3 to HEX0)
void Segs7_init(void){
	/*volatile uint32_t *direction_hex = PIO1_REG(DIRECTION_OFFSET);
	*direction_hex |= HEX3_BITS | HEX2_BITS | HEX1_BITS | HEX0_BITS;

	volatile uint32_t *value_hex = PIO1_REG(DATA_OFFSET);
	Seg7_write_hex(0, 0);
	Seg7_write_hex(1, 0);
	Seg7_write_hex(2, 0);
	Seg7_write_hex(3, 0);*/
	
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
	*value_leds ^= (maskleds << LED9) & LEDS_BITS;
}

bool Key_read(int key_number)
{
	volatile uint32_t *dataValue = BOUTON_REG;
	uint32_t my_key_value = (*dataValue >> key_number) & 0x1;
	return my_key_value;
}

// Variable to store the state of the keys
bool positive_edge[4] = {true, true, true, true};
bool Key_read_edge(int key_number)
{
	volatile uint32_t *dataValue = BOUTON_REG;
	// Get the key number and check its value
	uint32_t my_key_value = (*dataValue >> key_number) & 0x1;

	if (my_key_value == 1 && !positive_edge[key_number])
	{									  // Key is pressed (active low) and no edge detected yet
		positive_edge[key_number] = true; // Mark the positive edge (key press) as detected
		return true; // Rising edge (key press) detected
	}
	else if (my_key_value == 0 && positive_edge[key_number])
	{									   // Key is released (active low)
		positive_edge[key_number] = false; // Reset the edge detection for the next press
	}

	return false;
}

bool Max10_check_status(void)
{
    volatile uint32_t *status = STATUS_REG;
    return ((*status & STATUS_VALID) == STATUS_VALID);
}

/*int Max10_check_WE(void){
	volatile uint32_t *we_reg = LP36_WE_REG;

	return *we_reg;
}*/

/*int Max10_read_sel(void){
	volatile uint32_t *sel_reg = LP36_WE_REG;

	return *sel_reg;
}*/

void Max10_write_serial_link(uint32_t value, uint8_t code)
{
	volatile uint32_t *sel_reg = SERIAL_LINK_CODE_REG;
	volatile uint32_t *data_reg = SERIAL_LINK_DATA_REG;

    if (code > 0x0F)
    {
        printf("Erreur : Valeur de sel invalide.\n");
        return;
    }
    *sel_reg = code << SERIAL_LINK_CODE_SHIFT;
    *data_reg = value << SERIAL_LINK_DATA_SHIFT;
}

/*uint32_t Max10_read_leds(uint8_t sel)
{
	volatile uint32_t *sel_reg = SERIAL_LINK_CODE_REG;
	volatile uint32_t *data_reg = SERIAL_LINK_DATA_REG;

    if (sel > 0x0F)
    {
        printf("Erreur : Valeur de sel invalide.\n");
        return 0;
    }


    *sel_reg = sel;
    return *data_reg;
}*/

/*void Max10_write_all(uint32_t value){

		for(uint32_t i = 0; i < NUM_MODE; i++){
			Max10_write_leds(value,i);
		}
}*/

void enable_counter(){
	volatile uint32_t *enable = ENABLE_COUNTER_REG;
	*enable |= ENABLE_COUNTER_MASK ;
}

void disable_counter(){
	volatile uint32_t *disable = ENABLE_COUNTER_REG;
	*disable &= ~ENABLE_COUNTER_MASK;
}

uint32_t counter_current_value(){
	volatile uint32_t *value = COUNTER_CURRENT_VALUE_REG;
	return *value;
}

void reset_counter(){
	volatile uint32_t *reset = RESET_COUNTER_REG;
	*reset |= RESET_COUNTER_MASK;
}
