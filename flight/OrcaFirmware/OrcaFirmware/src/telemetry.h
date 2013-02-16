/**************************************************************************
 * \file telemetry.h
 *
 * \brief Header file for telemetry.c
 *
 * Created: 02.02.2013 12:51:54
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/


#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <asf.h>
#include "user_board.h"

//////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////

#define TELEMETRY_USART_INTERFACE			BOARD_USART_USB_INTERFACE
#define TELEMETRY_USART_BAUDRATE			9600
#define TELEMETRY_USART_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define TELEMETRY_USART_PARITY              USART_PMODE_DISABLED_gc
#define TELEMETRY_USART_STOP_BIT            true


//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////

uint8_t telemetry_init();
void telemetry_task();

#endif /* TELEMETRY_H_ */