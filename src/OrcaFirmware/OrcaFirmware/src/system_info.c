/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \\file *********************************************************************
 *
 * \\brief
 *			System Info source file.
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
 * 		system_info.c \\n
 * \\par Date
 * 		 05.06.2012  \\n
 * \\todo
 *
 * \\par History
 * 		 -v0.01 first version
 *****************************************************************************/

#include "system_info.h"

 /*************************************************************************
* \\brief Process Info Message
*
*
* \\param *infocode		The Error Code
* \\param *errorlvl		The Error lvl
*
*  \return  ---
***************************************************************************/
 void process_info_command(enum system_message infocode,enum errlvl errorlvl)
 {
 	
 	/* This is just logging message */
 	if(errorlvl == ERR_LVL_LOW)
 	{	
		//TODO: Logging
		nop();
 	}
 	
 	/* Save the last error and enter failsafe mode */
 	if(errorlvl == ERR_LVL_MID)
	{
		//TODO: Logging, Failsafe Mode
		nop();
	}
	
	/* Save the last error and restart the controller */
	if(errorlvl == ERR_LVL_HIGH)
	{
		//TODO: Logging, Resart
		nop();
	}
 }