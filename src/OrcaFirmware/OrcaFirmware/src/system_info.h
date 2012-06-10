/*
 * system_info.h
 *
 * Created: 29.05.2012 21:00:13
 *  Author: f-ruml
 */ 


#ifndef SYSTEM_INFO_H_
#define SYSTEM_INFO_H_

#ifndef nop()
#define nop()   do { __asm__ __volatile__ ("nop"); } while (0)
#endif

/*! \enum message
 * All information, error and state messages.
 */
enum system_message{
	SYSTEM_INFO_FALSE						=0x0000,	/*!< brief */
	SYSTEM_INFO_TRUE						=0x0001,	/*!< brief */
	/* Flight Controller */
	SYSTEM_INFO_WRONG_FLIGHT_MODE			=0x0010,	/*!< brief RC Controll is not possible. False mode selected. */
	SYSTEM_INFO_NO_FLIGHT_MODE				=0x0011,	/*!< brief No Flight Mode selected */
	/* Servo Input */
	SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE		=0x0020,	/*!< brief Servo Input value is inadmissible. */
};

/*! \enum errlvl
 * Error lvl's.
 */
enum errlvl{
	ERR_LVL_LOW,	/*!< brief notice Error, keep running */
	ERR_LVL_MID,	/*!< brief notice Error, failsafe mode */
	ERR_LVL_HIGH	/*!< brief notice Error, restart! */
};

void process_info_command(enum system_message infocode,enum errlvl errorlvl);
 
#endif /* SYSTEM_INFO_H_ */