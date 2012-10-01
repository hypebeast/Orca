/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *			This file contains the board definitions.
 *
 *
 *
 * \par Application note:
 *
 * \author
 *      Sebastian Ruml, Fabian Ruml
 *
 * \par Device
 * 		ATxmega 128A1
 * \par File
 * 		main.h \n
 * \par Date
 * 		 2012.02.12  \n
 *
 * \par History
 * 		 - v0.0.1 first version
 *****************************************************************************/

#ifndef BOARD_H_
#define BOARD_H_

#ifndef F_CPU
/*! \brief Define default CPU frequency, if this is not already defined. */
#define F_CPU 32000000UL
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>


//---------------------------------------------------------------------
// Allgemeine Funktionen
//---------------------------------------------------------------------
/*! \brief This macro will protect the following code from interrupts. */
#define AVR_ENTER_CRITICAL_REGION( ) uint8_t volatile saved_sreg = SREG; \
                                     cli();

/*! \brief This macro must always be used in conjunction with AVR_ENTER_CRITICAL_REGION
 *        so the interrupts are enabled again.
 */
#define AVR_LEAVE_CRITICAL_REGION( ) SREG = saved_sreg;

/*! \brief Define the delay_us macro for GCC. */
#define delay_us( us )   (_delay_us( us ))
/*! \brief Define the delay_ms macro for GCC. */
#define delay_ms( ms )   (_delay_ms( ms ))

#define INLINE static inline

/*! \brief Define the no operation macro. */
#define nop()   do { __asm__ __volatile__ ("nop"); } while (0)
#define MAIN_TASK_PROLOGUE int
#define MAIN_TASK_EPILOGUE() return -1;
#define SHORTENUM __attribute__ ((packed))

/*-------------I/O Operationen-------------*/
#define PORT_SetPinsAsInput( _port, _inputMask) 	( (_port)->DIRCLR = _inputMask )
#define PORT_SetPinsAsOutput( _port, _outputMask )	( (_port)->DIRSET = _outputMask )
#define PORT_SetPins( _port, _setMask) 				( (_port)->OUTSET = _setMask )
#define PORT_ClearPins( _port, _clearMask) 			( (_port)->OUTCLR = _clearMask )
#define PORT_GetPortValue( _port )					( (_port)->IN )

/*-------------Clock-----------------------*/
#define CLKSYS_Enable( _oscSel ) ( OSC.CTRL |= (_oscSel) )
#define CLKSYS_IsReady( _oscSel ) ( OSC.STATUS & (_oscSel) )


//---------------------------------------------------------------------
//	Servo definitions
//---------------------------------------------------------------------

#define SERVO_EN_PIN				PIN7_bm					/*!< \brief SPI RTS PIN*/
#define SERVO_EN_PORT				&PORTE					/*!< \brief SPI RTS PORT*/
#define SERVO_EN_ON()				( PORT_SetPins(SERVO_EN_PORT, SERVO_EN_PIN) )	/*!< \brief Enable Servo*/
#define SERVO_EN_OFF()				( PORT_ClearPins(SERVO_EN_PORT, SERVO_EN_PIN) )	/*!< \brief Disable Servo*/

#define SERVO_1_PIN					PIN4_bm					/*!< \brief Servo 1 PIN*/
#define SERVO_1_PORT				&PORTC					/*!< \brief Servo 1 PORT*/
#define SERVO_2_PIN					PIN5_bm					/*!< \brief Servo 2 PIN*/
#define SERVO_2_PORT				&PORTC					/*!< \brief Servo 2 PORT*/
#define SERVO_3_PIN					PIN6_bm					/*!< \brief Servo 3 PIN*/
#define SERVO_3_PORT				&PORTC					/*!< \brief Servo 3 PORT*/
#define SERVO_4_PIN					PIN7_bm					/*!< \brief Servo 4 PIN*/
#define SERVO_4_PORT				&PORTC					/*!< \brief Servo 4 PORT*/
#define SERVO_5_PIN					PIN4_bm					/*!< \brief Servo 5 PIN*/
#define SERVO_5_PORT				&PORTD					/*!< \brief Servo 5 PORT*/
#define SERVO_6_PIN					PIN5_bm					/*!< \brief Servo 6 PIN*/
#define SERVO_6_PORT				&PORTD					/*!< \brief Servo 6 PORT*/
#define SERVO_7_PIN					PIN6_bm					/*!< \brief Servo 7 PIN*/
#define SERVO_7_PORT				&PORTD					/*!< \brief Servo 7 PORT*/
#define SERVO_8_PIN					PIN7_bm					/*!< \brief Servo 8 PIN*/
#define SERVO_8_PORT				&PORTD					/*!< \brief Servo 8 PORT*/

#define STAT_LED_PIN				PIN2_bm					/*!< \brief SPI RTS PIN*/
#define STAT_LED_PORT				&PORTK					/*!< \brief SPI RTS PORT*/
#define STAT_LED_OFF()				( PORT_ClearPins(STAT_LED_PORT, STAT_LED_PIN) )
#define STAT_LED_ON()				( PORT_SetPins(STAT_LED_PORT, STAT_LED_PIN) )

/*! \brief Acceleration sensor struct
 *
 *  Acceleration sensor struct.
 */
typedef struct FLIGHT_DATA {
	int16_t	pitch;                 /*!< roll */
	int16_t	roll;                  /*!< pitch */
}FLIGHT_DATA_t;

#endif /* BOARD_H_ */
