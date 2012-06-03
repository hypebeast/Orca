/*
 * orca_init.c
 *
 * Created: 07.04.2012 22:27:17
 *  Author: f-ruml
 */ 

#include <ioport.h>
#include "orca_init.h"
#include"orca_board.h"

 /***********************************************************************//**
* \brief Board Init
* 
*  Orca board init.
*
* \note 
*
* \param *copley		Copley Datenstruktur
* \param *canOpenMaster	CANopen Master Objekt
*
*  return  ---
***************************************************************************/
void orca_board_init(BOARD_CONFIG_t *board)
{
	board->extensionBoards = 0x00;
	
	/* configure all inputs and outputs*/
	ioport_configure_pin(BOARD_LED_ERR_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_LED_STAT_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(BOARD_EXTENSION_POWER_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_EXTENSION_INS_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_EXTENSION_INTERFACE_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_EXTENSION_SPARE_1_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_EXTENSION_SPARE_2_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_EXTENSION_SPARE_3_BOARD, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		
	ioport_configure_pin(BOARD_PUSH_BUTTON_1_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_2_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_3_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_4_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_5_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_6_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_7_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BOARD_PUSH_BUTTON_8_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
	ioport_configure_pin(BOARD_SERVO_IN_1_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_2_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_3_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_4_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_5_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_6_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);
	ioport_configure_pin(BOARD_SERVO_IN_7_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN | IOPORT_BOTHEDGES);

	ioport_configure_pin(BOARD_SERVO_OUT_1_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_SERVO_OUT_2_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_SERVO_OUT_3_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_SERVO_OUT_4_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_SERVO_OUT_5_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(BOARD_SERVO_OUT_6_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	//PORT_ConfigureInterrupt0( &PORTH, PORT_INT0LVL_MED_gc, 0x01 );
	
	
	/* check for extension boards. The corresponding pin is low if the extension board is used. */
	if(ioport_pin_is_low(BOARD_EXTENSION_POWER_BOARD))
		board->extensionBoards &= BOARD_EXTENSION_POWER_BOARD_USED;
		
	if(ioport_pin_is_low(BOARD_EXTENSION_INS_BOARD))
		board->extensionBoards &= BOARD_EXTENSION_INS_BOARD_USED;
		
	if(ioport_pin_is_low(BOARD_EXTENSION_INTERFACE_BOARD))
		board->extensionBoards &= BOARD_EXTENSION_INTERFACE_BOARD;
	//note: Add here additional extension boards!
	
		/* check the selected modes */
	if(ioport_pin_is_low(BOARD_PUSH_BUTTON_1_GPIO))
		board->mode |= BOARD_MODE_TRANSMITTER;		
	//note: Add here additional modes!
	
	
	/* Configure the used pins for the serial data flash */
	ioport_configure_pin(BOARD_SERIAL_DATA_FLASH_MASTER_SCK, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_SERIAL_DATA_FLASH_MASTER_MOSI, IOPORT_DIR_OUTPUT |IOPORT_INIT_HIGH);
	ioport_configure_pin(BOARD_SERIAL_DATA_FLASH_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(BOARD_SERIAL_DATA_FLASH_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
}