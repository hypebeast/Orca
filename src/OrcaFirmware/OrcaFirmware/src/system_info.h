/*
 * system_info.h
 *
 * Created: 29.05.2012 21:00:13
 *  Author: f-ruml
 */ 


#ifndef SYSTEM_INFO_H_
#define SYSTEM_INFO_H_

/*! \enum message
 * All information, error and state messages.
 */
enum message{
	SYSTEM_INFO_FALSE=0x00,				/*!< brief */
	SYSTEM_INFO_TRUE=0x01,				/*!< brief */
	/* Flight Controller */
	SYSTEM_INFO_WRONG_FLIGHT_MODE,		/*!< brief RC Controll is not possible. False mode selected. */
	SYSTEM_INFO_NO_FLIGHT_MODE,			/*!< No Flight Mode selected */
};

/*! \enum errlvl
 * Error lvl's.
 */
enum errlvl{
	ERR_LVL_NONE,					/*!< Just for logging */
	ERR_LVL_MID,					/*!< notice Error, keep running */
	ERR_LVL_HIGH					/*!< notice Error, abort! */
};

#endif /* SYSTEM_INFO_H_ */