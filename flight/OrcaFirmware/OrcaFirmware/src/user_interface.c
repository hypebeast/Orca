/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \\file *********************************************************************
 *
 * \\brief
 *			User interface file./n
 *
 *			
 *
 *
 * \\par Application note:
 *
 * \\author
 *      f-ruml
 *
 * \\par Device
 * 		ATxmega 128A1 \n
 * \\par File
 * 		user_interface.c \\n
 * \\par Date
 * 		 04.10.2012  \\n
 * \\todo
 *
 * \\par History
 * 		 -v0.01 first version
 *****************************************************************************/

#include "sysclk.h"
#include "user_board.h"
#include "user_interface.h"
#include "system_info.h"

//////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////
uint8_t errLed = USER_INTERFACE_LED_OFF;
uint8_t statLed = USER_INTERFACE_LED_OFF;
uint8_t statLedcounter = 0;
uint8_t errLedcounter = 0;

/**************************************************************************
* \brief Status LED pattern
*	Call this method to select a new status LED pattern.
*
* \param *pattern the new LED pattern, definied in user_interface.h
*
* \return  true, if pattern is supported
***************************************************************************/
uint16_t user_interface_stat_led_pattern(uint8_t pattern)
{
	if(pattern == USER_INTERFACE_LED_OFF || pattern == USER_INTERFACE_LED_BLINKING ||
		pattern == USER_INTERFACE_LED_SINGLE_FLASH || pattern == USER_INTERFACE_LED_DOUBLE_FLASH ||
		pattern == USER_INTERFACE_LED_TRIPLE_FLASH || pattern == USER_INTERFACE_LED_ON)
		{
			statLed = pattern;
			statLedcounter = 0;
			return SYSTEM_INFO_TRUE;
		}
	else
		return SYSTEM_INFO_UI_LED_PATTERN_ERR;
}

/**************************************************************************
* \brief Error LED pattern
*	Call this method to select a new error LED pattern.
*
* \param *pattern the new LED pattern, definied in user_interface.h
*
* \return  true, if pattern is supported
***************************************************************************/
uint16_t user_interface_err_led_pattern(uint8_t pattern)
{
	if(pattern == USER_INTERFACE_LED_OFF || pattern == USER_INTERFACE_LED_BLINKING ||
	pattern == USER_INTERFACE_LED_SINGLE_FLASH || pattern == USER_INTERFACE_LED_DOUBLE_FLASH ||
	pattern == USER_INTERFACE_LED_TRIPLE_FLASH || pattern == USER_INTERFACE_LED_ON)
	{
		errLed = pattern;
		errLedcounter = 0;
		return SYSTEM_INFO_TRUE;
	}
	else
		return SYSTEM_INFO_UI_LED_PATTERN_ERR;
}

/**************************************************************************
* \brief Update LEDs
*	This method updates the stats and error LEDs. /n
*	This method must be called every 50 ms. /n
*
* \param  ---
*
* \return  ---
***************************************************************************/
void user_interface_update_LEDs(void)
{
	/* Status LED */
	switch (statLed)
	{
		case USER_INTERFACE_LED_OFF:
			STAT_LED_OFF();
		break;
	
		case USER_INTERFACE_LED_ON:
			STAT_LED_ON();
		break;
		
		case USER_INTERFACE_LED_BLINKING:
			if(statLedcounter <= 4)
			{
				STAT_LED_ON();
				statLedcounter++;
			}
			else if (statLedcounter <= 8)
			{
				STAT_LED_OFF();
				statLedcounter++;
			}
			else
			statLedcounter = 0;
		break;
	
		case USER_INTERFACE_LED_SINGLE_FLASH:
			if(statLedcounter <= 4)
			{
				STAT_LED_ON();
				statLedcounter++;
			}
			else if (statLedcounter <= 24)
			{
				STAT_LED_OFF();
				statLedcounter++;
			}
			else
			statLedcounter = 0;
		break;
		
		case USER_INTERFACE_LED_DOUBLE_FLASH:
		    if(statLedcounter <= 4)
		    {
			    STAT_LED_ON();
			    statLedcounter++;
		    }
		    else if (statLedcounter <= 8)
		    {
			    STAT_LED_OFF();
			    statLedcounter++;
		    }
		    else if(statLedcounter <= 12)
		    {
			    STAT_LED_ON();
			    statLedcounter++;
		    }
		    else if (statLedcounter <= 32)
		    {
			    STAT_LED_OFF();
			    statLedcounter++;
		    }
		    else
		    statLedcounter = 0;
		break;
		
		case USER_INTERFACE_LED_TRIPLE_FLASH:
			if(statLedcounter <= 4)
			{
				STAT_LED_ON();
				statLedcounter++;
			}
			else if (statLedcounter <= 8)
			{
				STAT_LED_OFF();
				statLedcounter++;
			}
			else if(statLedcounter <= 12)
			{
				STAT_LED_ON();
				statLedcounter++;
			}
			else if (statLedcounter <= 16)
			{
				STAT_LED_OFF();
				statLedcounter++;
			}
			else if(statLedcounter <= 20)
			{
				STAT_LED_ON();
				statLedcounter++;
			}
			else if (statLedcounter <= 40)
			{
				STAT_LED_OFF();
				statLedcounter++;
			}
			else
			statLedcounter = 0;
		break;	
	}

    
    /* Error LED */
	switch (errLed)
	{
		case USER_INTERFACE_LED_OFF:
		ERR_LED_OFF();
		break;
		
		case USER_INTERFACE_LED_ON:
		ERR_LED_ON();
		break;
		
		case USER_INTERFACE_LED_BLINKING:
		if(errLedcounter <= 4)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 8)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else
		errLedcounter = 0;
		break;
		
		case USER_INTERFACE_LED_SINGLE_FLASH:
		if(errLedcounter <= 4)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 24)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else
		errLedcounter = 0;
		break;
		
		case USER_INTERFACE_LED_DOUBLE_FLASH:
		if(errLedcounter <= 4)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 8)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else if(errLedcounter <= 12)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 32)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else
		errLedcounter = 0;
		break;
		
		case USER_INTERFACE_LED_TRIPLE_FLASH:
		if(errLedcounter <= 4)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 8)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else if(errLedcounter <= 12)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 16)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else if(errLedcounter <= 20)
		{
			ERR_LED_ON();
			errLedcounter++;
		}
		else if (errLedcounter <= 40)
		{
			ERR_LED_OFF();
			errLedcounter++;
		}
		else
		errLedcounter = 0;
		break;
	}
}
