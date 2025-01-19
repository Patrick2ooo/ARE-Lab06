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
 * Brief: Header file for avalon function
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 0.0    09.11.2024  RAD & PAM     Initial version.
 *
*****************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "axi_lw.h"


// Base address
#define H2F_BASE_ADD AXI_LW_HPS_FPGA_BASE_ADD
#define INTERFACE_BASE_ADD 0xFF210000

// ACCESS MACROS
#define INTERFACE_REG(_x_) ((volatile uint32_t *)(INTERFACE_BASE_ADD + _x_)) // _x_ is an offset with respect to the base address
//#define H2F(_x_)   (volatile uint32_t *)(H2F_BASE_ADD + _x_)

// Offset interface
#define ID_OFFSET       0x0
#define LED_OFFSET      0xc
#define SWITCH_OFFSET   0x8
#define BOUTON_OFFSET   0x4
#define HEX_OFFSET      0x10

// Our added adress (TO CHECK IF STILL VALID OR USED)
#define CON_80P_STATUS_OFFSET           0x14
#define SERIAL_LINK_READY_OFFSET        0x14
#define SERIAL_LINK_CODE_OFFSET         0x14
#define SERIAL_LINK_DATA_OFFSET         0x14
#define COUNTER_CURRENT_VALUE_OFFSET    0x18
#define ENABLE_COUNTER_OFFSET           0x18
#define RESET_COUNTER_OFFSET            0x18
#define IRQ_FLAG_OFFSET                 0x1c
#define CLEAR_IRQ_OFFSET                0x1c

/*#define LP36_STATUS_OFFSET  0x10
#define LP36_WE_OFFSET      0x14
#define LP36_DATA_OFFSET    0x18
#define LP36_SEL_OFFSET     0x1C*/

// A REVERIFIER
#define HEX3_BITS    	0x0FE00000
#define HEX2_BITS    	0x001FC000
#define HEX1_BITS    	0x00003F80
#define HEX0_BITS    	0x0000007F

#define ID_REG      INTERFACE_REG(ID_OFFSET)
#define LED_REG     INTERFACE_REG(LED_OFFSET)
#define SWITCH_REG  INTERFACE_REG(SWITCH_OFFSET)
#define BOUTON_REG  INTERFACE_REG(BOUTON_OFFSET)
#define STATUS_REG  INTERFACE_REG(CON_80P_STATUS_OFFSET)

#define SERIAL_LINK_CODE_REG        INTERFACE_REG(CON_80P_STATUS_OFFSET)
#define SERIAL_LINK_READY_REG       INTERFACE_REG(SERIAL_LINK_READY_OFFSET)
#define SERIAL_LINK_DATA_REG        INTERFACE_REG(SERIAL_LINK_CODE_OFFSET)
#define COUNTER_CURRENT_VALUE_REG   INTERFACE_REG(COUNTER_CURRENT_VALUE_OFFSET)
#define ENABLE_COUNTER_REG          INTERFACE_REG(ENABLE_COUNTER_OFFSET)
#define RESET_COUNTER_REG           INTERFACE_REG(RESET_COUNTER_OFFSET)
#define IRQ_FLAG_REG                INTERFACE_REG(IRQ_FLAG_OFFSET)
#define CLEAR_IRQ_REG               INTERFACE_REG(CLEAR_IRQ_OFFSET)

//#define LP36_STATUS_REG     INTERFACE_REG(LP36_STATUS_OFFSET)
//#define LP36_WE_REG         INTERFACE_REG(LP36_WE_OFFSET)
//#define LP36_DATA_REG       INTERFACE_REG(LP36_DATA_OFFSET)
//#define LP36_SEL_REG        INTERFACE_REG(LP36_SEL_OFFSET)

#define SWITCHS_BITS    0x000003FF
#define LEDS_BITS       0x000003FF
#define BOUTON_BITS     0x0000000F

#define SERIAL_LINK_CODE_MASK     0x000F0000
#define SERIAL_LINK_DATA_MASK     0x0000FFFF
#define SERIAL_LINK_READY_MASK    0x00000004

#define ENABLE_COUNTER_MASK      0x00000002
#define RESET_COUNTER_MASK       0x00000001

#define IRQ_CLEAR_MASK  0x00000001
#define IRQ_MASK_MASK   0x00000002

#define SWITCH0_MASK    0x00000001
#define SWITCH1_MASK    0x00000002
#define SWITCH2_MASK    0x00000004
#define SWITCH3_MASK    0x00000008

#define SERIAL_LINK_CODE_SHIFT    16
#define SERIAL_LINK_DATA_SHIFT    0

#define ENABLE_COUNTER_SHIFT      1
#define RESET_COUNTER_SHIFT       0

#define LED0 (uint32_t)(1 << 0)
#define LED1 (uint32_t)(1 << 1)
#define LED2 (uint32_t)(1 << 2)
#define LED3 (uint32_t)(1 << 3)
#define LED4 (uint32_t)(1 << 4)
#define LED5 (uint32_t)(1 << 5)
#define LED6 (uint32_t)(1 << 6)
#define LED7 (uint32_t)(1 << 7)
#define LED8 (uint32_t)(1 << 8)
#define LED9 (uint32_t)(1 << 9)

#define STATUS_VALID 0x01

#define NUM_MODE 4

#define LED_OFF 0x0

#define WAIT_DISPLAY    0x00847C84
#define BEGIN_DISPLAY   0x03F1463F
#define END_DISPLAY     0x000E39C0

uint32_t generate_random(void);

// Leds_write function : Write a value to all Leds (LED9 to LED0)
// Parameter : "value"= data to be applied to all Leds
// Return : None
void Leds_write(uint32_t value);

// Leds_set function : Set to ON some or all Leds (LED9 to LED0)
// Parameter : "maskleds"= Leds selected to apply a set (maximum 0x3FF)
// Return : None
void Leds_set(uint32_t maskleds);

// Leds_read function : Read the value of all Leds (LED9 to LED0)
// Parameter : "led"= mask to select the Leds to read
// Return : Value of the Led
uint32_t Leds_read(uint32_t led);

// Leds_toggle function : Toggle the curent value of some or all Leds (LED9 to LED0)
// Parameter : "maskleds"= Leds selected to apply a toggle (maximum 0x3FF)
// Return : None
void Leds_toggle(uint32_t maskleds);

// Key_read function : Read one Key status, pressed or not (KEY0 or KEY1 or KEY2 or KEY3)
// Parameter : "key_number"= select the key number to read, from 0 to 3
// Return : True(1) if key is pressed, and False(0) if key is not pressed
bool Key_read(int key_number);

// Key_read_edge function : Read one Key status, pressed or not but only on the rising edge 
// Parameter : "key_number"= select the key number to read, from 0 to 3
// Return : True(1) if key is pressed, and False(0) if key is not pressed
bool Key_read_edge(int key_number);

// Switchs_read function : Read the switchs value
// Parameter : None
// Return : Value of all Switchs (SW9 to SW0)
uint32_t Switchs_read(void);

bool serial_transmitter_ready();

// Max10_check_status function : Check the status of the LP36
// Parameter : None
// Return : True(1) if the status is valid, and False(0) if the status is not valid
bool Max10_check_status(void);

// Max10_write_leds function : Write a value to the LP36
// Parameter1 : "value"= data to be applied to the LP36
// Parameter2 : "sel"= select the leds to write
// Return : None
void Max10_write_serial_link(uint32_t value, uint8_t sel);

// Max10_read_leds function : Read the value of the LP36
// Parameter : "sel"= select the leds to read
// Return : Value of the leds
//uint32_t Max10_read_leds(uint8_t sel);

// Max10_write_all function : Write a value to all the leds 
// Parameter : "value"= data to be applied to all the leds of the LP36
// Return : None
// void Max10_write_all(uint32_t value);

// Segs7_init function : Initialize all 7-segments display in PIO core (HEX3 to HEX0)
void Segs7_init(void);

// Seg7_write function : Write digit segment value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= digit segment value to be applied on the selected 7-segments (maximum 0x7F to switch ON all segments)
// Return : None
void Seg7_write(int seg7_number, uint32_t value);

// Seg7_write_hex function : Write an Hexadecimal value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= Hexadecimal value to be display on the selected 7-segments, form 0x0 to 0xF
// Return : None
void Seg7_write_hex(int seg7_number, uint32_t value);

void enable_counter();

void disable_counter();

uint32_t counter_current_value();

void reset_counter();

void clear_irq();

void Max10_init(void);

void new_time();

void new_attemps();

void start_game();

void Max10_write_square(uint32_t value);

bool is_counter_enabled();

bool new_error();

void Seg7_display(uint32_t switch_value);