/*
 * user_interface.h
 *
 * Created: 04.10.2012 09:37:45
 *  Author: f-ruml
 */ 
#include <ioport.h>

#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

//---------------------------------------------------------------------
//	Possible LED Patterns
//---------------------------------------------------------------------
#define USER_INTERFACE_LED_OFF				0x00
#define USER_INTERFACE_LED_BLINKING			0x01
#define USER_INTERFACE_LED_SINGLE_FLASH		0x02
#define USER_INTERFACE_LED_DOUBLE_FLASH		0x03
#define USER_INTERFACE_LED_TRIPLE_FLASH		0x04
#define USER_INTERFACE_LED_ON				0x05

//---------------------------------------------------------------------
//	LED Macros
//---------------------------------------------------------------------
#define STAT_LED_ON()			ioport_set_pin_high(BOARD_LED_STAT_GPIO)
#define STAT_LED_OFF()			ioport_set_pin_low(BOARD_LED_STAT_GPIO)
#define ERR_LED_ON()			ioport_set_pin_high(BOARD_LED_ERR_GPIO)
#define ERR_LED_OFF()			ioport_set_pin_low(BOARD_LED_ERR_GPIO)

void user_interface_update_LEDs(void);
uint16_t user_interface_stat_led_pattern(uint8_t pattern);
uint16_t user_interface_err_led_pattern(uint8_t pattern);
#endif /* USER_INTERFACE_H_ */