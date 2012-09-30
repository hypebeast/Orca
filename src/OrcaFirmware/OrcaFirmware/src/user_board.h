
/**
 * \file user_board.h
 *
 * \brief Header file for board definitions
 *
 * Copyright (c) 2012 Fabian Ruml <fabian.ruml@gmail.com>, Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */

#ifndef ORCA_BOARD_H_
#define ORCA_BOARD_H_

/*! \brief Define the no operation macro. */
#define nop()   do { __asm__ __volatile__ ("nop"); } while (0)

	
//---------------------------------------------------------------------
//	GPIO Connections Of PWM Servo Output
// Servo Output mapping :
// *Servo 1 -> EDF left
// *Servo 2 -> EDF right
// *Servo 3 -> Servo left
// *Servo 4 -> Servo right
// *Servo 5 -> EDF rear
// *Servo 6 -> Out 6
//---------------------------------------------------------------------
//#define BOARD_SERVO_OUT_1_PIN		PIN4_bm							/*!< \brief PWM Servo 1 Out PIN*/
//#define BOARD_SERVO_OUT_1_PORT		&PORTC						/*!< \brief PWM Servo 1 Out PORT*/

//#define BOARD_SERVO_OUT_1_GPIO		IOPORT_CREATE_PIN(PORTC, 4)		/*!< \brief PWM Servo 1 Out */
//#define BOARD_SERVO_OUT_2_GPIO		IOPORT_CREATE_PIN(PORTC, 5)		/*!< \brief PWM Servo 2 Out */
//#define BOARD_SERVO_OUT_3_GPIO		IOPORT_CREATE_PIN(PORTD, 4)		/*!< \brief PWM Servo 3 Out */
//#define BOARD_SERVO_OUT_4_GPIO		IOPORT_CREATE_PIN(PORTD, 5)		/*!< \brief PWM Servo 4 Out */
//#define BOARD_SERVO_OUT_5_GPIO		IOPORT_CREATE_PIN(PORTE, 4)		/*!< \brief PWM Servo 5 Out */

#define BOARD_SERVO_OUT_EDF_LEFT_GPIO		IOPORT_CREATE_PIN(PORTC, 4)		/*!< \brief PWM EDF Left Out */
#define BOARD_SERVO_OUT_EDF_RIGHT_GPIO		IOPORT_CREATE_PIN(PORTC, 5)		/*!< \brief PWM EDF Right Out */
#define BOARD_SERVO_OUT_SERVO_LEFT			IOPORT_CREATE_PIN(PORTD, 4)		/*!< \brief PWM Servo Left Out */
#define BOARD_SERVO_OUT_SERVO_RIGHT			IOPORT_CREATE_PIN(PORTD, 5)		/*!< \brief PWM Servo Right Out */
#define BOARD_SERVO_OUT_EDF_REAR			IOPORT_CREATE_PIN(PORTE, 4)		/*!< \brief PWM Servo EDF Rear Out */
#define BOARD_SERVO_OUT_6_GPIO				IOPORT_CREATE_PIN(PORTE, 5)		/*!< \brief PWM Servo 6 Out */

//---------------------------------------------------------------------
//	GPIO Connections Of Servo Input
//---------------------------------------------------------------------
#define BOARD_SERVO_IN_PORT			PORTH
#define BOARD_SERVO_IN_1_PIN_bm		0x01
#define BOARD_SERVO_IN_2_PIN_bm		0x02
#define BOARD_SERVO_IN_3_PIN_bm		0x04
#define BOARD_SERVO_IN_4_PIN_bm		0x08
#define BOARD_SERVO_IN_5_PIN_bm		0x10
#define BOARD_SERVO_IN_6_PIN_bm		0x20
#define BOARD_SERVO_IN_7_PIN_bm		0x40

#define BOARD_SERVO_IN_1_GPIO		IOPORT_CREATE_PIN(PORTH, 0)		/*!< \brief Servo 1 IN */
#define BOARD_SERVO_IN_2_GPIO		IOPORT_CREATE_PIN(PORTH, 1)		/*!< \brief Servo 2 IN */
#define BOARD_SERVO_IN_3_GPIO		IOPORT_CREATE_PIN(PORTH, 2)		/*!< \brief Servo 3 IN */
#define BOARD_SERVO_IN_4_GPIO		IOPORT_CREATE_PIN(PORTH, 3)		/*!< \brief Servo 4 IN */
#define BOARD_SERVO_IN_5_GPIO		IOPORT_CREATE_PIN(PORTH, 4)		/*!< \brief Servo 5 IN */
#define BOARD_SERVO_IN_6_GPIO		IOPORT_CREATE_PIN(PORTH, 5)		/*!< \brief Servo 6 IN */
#define BOARD_SERVO_IN_7_GPIO		IOPORT_CREATE_PIN(PORTH, 6)		/*!< \brief Servo 7 IN */

//---------------------------------------------------------------------
//	GPIO Connections Of LEDs
//---------------------------------------------------------------------
#define BOARD_LED_ERR_GPIO		IOPORT_CREATE_PIN(PORTK, 7)		/*!< \brief Error LED */
#define BOARD_LED_STAT_GPIO		IOPORT_CREATE_PIN(PORTK, 6)		/*!< \brief State LED */
#define Stat_LED_ON()			ioport_set_pin_high(BOARD_LED_STAT_GPIO)
#define Stat_LED_OFF()			ioport_set_pin_low(BOARD_LED_STAT_GPIO)
#define Err_LED_ON()			ioport_set_pin_high(BOARD_LED_ERR_GPIO)
#define Err_LED_OFF()			ioport_set_pin_low(BOARD_LED_ERR_GPIO)


//---------------------------------------------------------------------
//	GPIO Connections Of Push Buttons
//---------------------------------------------------------------------
#define BOARD_PUSH_BUTTON_1_GPIO		IOPORT_CREATE_PIN(PORTJ, 0)		/*!< \brief Push Button 1 */
#define BOARD_PUSH_BUTTON_2_GPIO		IOPORT_CREATE_PIN(PORTJ, 1)		/*!< \brief Push Button 2 */
#define BOARD_PUSH_BUTTON_3_GPIO		IOPORT_CREATE_PIN(PORTJ, 2)		/*!< \brief Push Button 3 */
#define BOARD_PUSH_BUTTON_4_GPIO		IOPORT_CREATE_PIN(PORTJ, 3)		/*!< \brief Push Button 4 */
#define BOARD_PUSH_BUTTON_5_GPIO		IOPORT_CREATE_PIN(PORTJ, 4)		/*!< \brief Push Button 5 */
#define BOARD_PUSH_BUTTON_6_GPIO		IOPORT_CREATE_PIN(PORTJ, 5)		/*!< \brief Push Button 6 */
#define BOARD_PUSH_BUTTON_7_GPIO		IOPORT_CREATE_PIN(PORTJ, 6)		/*!< \brief Push Button 7 */
#define BOARD_PUSH_BUTTON_8_GPIO		IOPORT_CREATE_PIN(PORTJ, 7)		/*!< \brief Push Button 8 */

//---------------------------------------------------------------------
//	GPIO Extension Boards
//	The corresponding input is low, if the expansion board is present
//---------------------------------------------------------------------
#define BOARD_EXTENSION_POWER_BOARD			IOPORT_CREATE_PIN(PORTK, 0)		/*!< \brief Power Board existing */
#define BOARD_EXTENSION_INS_BOARD			IOPORT_CREATE_PIN(PORTK, 1)		/*!< \brief Inertial Navigation System Board existing */
#define BOARD_EXTENSION_INTERFACE_BOARD		IOPORT_CREATE_PIN(PORTK, 2)		/*!< \brief Interface Board existing */
#define BOARD_EXTENSION_SPARE_1_BOARD		IOPORT_CREATE_PIN(PORTK, 3)		/*!< \brief Currently not used */
#define BOARD_EXTENSION_SPARE_2_BOARD		IOPORT_CREATE_PIN(PORTK, 4)		/*!< \brief Currently not used */
#define BOARD_EXTENSION_SPARE_3_BOARD		IOPORT_CREATE_PIN(PORTK, 5)		/*!< \brief Currently not used */

//---------------------------------------------------------------------
//	GPIO Connections Spare PINs And Other Things
//---------------------------------------------------------------------
#define BOARD_POWER_SWITCH					IOPORT_CREATE_PIN(PORTA, 0)		/*!< \brief Power Switch on Power Board */
#define BOARD_SPARE_PIN_2_GPIO				IOPORT_CREATE_PIN(PORTA, 1)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_3_GPIO				IOPORT_CREATE_PIN(PORTA, 2)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_4_GPIO				IOPORT_CREATE_PIN(PORTA, 3)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_5_GPIO				IOPORT_CREATE_PIN(PORTA, 4)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_6_GPIO				IOPORT_CREATE_PIN(PORTA, 5)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_7_GPIO				IOPORT_CREATE_PIN(PORTA, 6)		/*!< \brief Spare PIN */
#define BOARD_SPARE_PIN_8_GPIO				IOPORT_CREATE_PIN(PORTA, 7)		/*!< \brief Spare PIN */

//---------------------------------------------------------------------
//	GPIO Serial Data Flash
//---------------------------------------------------------------------
#define BOARD_SERIAL_DATA_FLASH_SPI           &USARTF0
#define BOARD_SERIAL_DATA_FLASH_CS            IOPORT_CREATE_PIN(PORTF,4)
#define BOARD_SERIAL_DATA_FLASH_MASTER_SCK    IOPORT_CREATE_PIN(PORTF,7)  // SCK as output
#define BOARD_SERIAL_DATA_FLASH_MASTER_MOSI   IOPORT_CREATE_PIN(PORTF,5)  // MOSI as output
#define BOARD_SERIAL_DATA_FLASH_MASTER_MISO   IOPORT_CREATE_PIN(PORTF,6)  // MISO as input
#define BOARD_SERIAL_DATA_FLASH_SPI_CLK_MASK  SYSCLK_PORT_F

//---------------------------------------------------------------------
//	GPIO I2C
//---------------------------------------------------------------------
#define BOARD_I2C_INTERN_INTERFACE			  &TWIC
#define BOARD_I2C_INTERN_INTERFACE_SDA		  IOPORT_CREATE_PIN(PORTC,0)
#define BOARD_I2C_INTERN_INTERFACE_SCL		  IOPORT_CREATE_PIN(PORTC,1)
#define BOARD_I2C_INTERN_INTERFACE_CLK_MASK   SYSCLK_PORT_C

#define BOARD_I2C_EXTERN_INTERFACE			  &TWID
#define BOARD_I2C_EXTERN_INTERFACE_SDA		  IOPORT_CREATE_PIN(PORTD,0)
#define BOARD_I2C_EXTERN_INTERFACE_SCL		  IOPORT_CREATE_PIN(PORTD,1)
#define BOARD_I2C_EXTERN_INTERFACE_CLK_MASK   SYSCLK_PORT_D

//---------------------------------------------------------------------
//	GPIO USART
//---------------------------------------------------------------------
#define BOARD_USART_XBEE_INTERFACE			  &USARTC0
#define BOARD_USART_XBEE_INTERFACE_RXD		  IOPORT_CREATE_PIN(PORTC,2)
#define BOARD_USART_XBEE_INTERFACE_TXD		  IOPORT_CREATE_PIN(PORTC,3)
#define BOARD_USART_XBEE_INTERFACE_CLK_MASK   SYSCLK_PORT_C

#define BOARD_USART_WIFI_INTERFACE			  &USARTD0
#define BOARD_USART_WIFI_INTERFACE_RXD		  IOPORT_CREATE_PIN(PORTD,2)
#define BOARD_USART_WIFI_INTERFACE_TXD		  IOPORT_CREATE_PIN(PORTD,3)
#define BOARD_USART_WIFI_INTERFACE_CLK_MASK   SYSCLK_PORT_D

#define BOARD_USART_USB_INTERFACE			  &USARTE0
#define BOARD_USART_USB_INTERFACE_RXD		  IOPORT_CREATE_PIN(PORTE,2)
#define BOARD_USART_USB_INTERFACE_TXD		  IOPORT_CREATE_PIN(PORTE,3)
#define BOARD_USART_USB_INTERFACE_CLK_MASK    SYSCLK_PORT_E

#define BOARD_USART_GPS_INTERFACE			  &USARTF0
#define BOARD_USART_GPS_INTERFACE_RXD		  IOPORT_CREATE_PIN(PORTF,2)
#define BOARD_USART_GPS_INTERFACE_TXD		  IOPORT_CREATE_PIN(PORTF,3)
#define BOARD_USART_GPS_INTERFACE_CLK_MASK    SYSCLK_PORT_F

//---------------------------------------------------------------------
//	Extension Boards 
//---------------------------------------------------------------------
#define BOARD_EXTENSION_POWER_BOARD_USED		(0x01<<0)
#define BOARD_EXTENSION_INS_BOARD_USED			(0x01<<1)
#define BOARD_EXTENSION_INTERFACE_BOARD_USED	(0x01<<2)

//---------------------------------------------------------------------
//	Selected Modes
//---------------------------------------------------------------------
#define BOARD_MODE_TRANSMITTER		(0x01<<0)


/*! Board struct */
typedef struct BOARD_CONFIG {
	uint8_t extensionBoards;					/*!< brief used extension boards */	
	uint16_t mode;								/*!< brief remember selected mode */							
}BOARD_CONFIG_t;

void orca_init(void);
void system_timer(uint32_t time);
uint16_t i2c_intern_init(void);

#endif /* ORCA_BOARD_H_ */