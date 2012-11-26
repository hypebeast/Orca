/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *			serial flash source file./n
 *
 *			
 *
 *
 * \par Application note:
 *
 * \author
 *      f-ruml
 *
 * \par Device
 * 		ATxmega 128A1 \n
 * \par File
 * 		serial_flash.c \n
 * \par Date
 * 		 05.10.2012  \n
 * \todo
 *
 * \par History
 * 		 -v0.01 first version
 *****************************************************************************/
#include "sysclk.h"
#include "string.h"
#include "at45dbx.h"
#include "serial_flash.h"
#include "filters.h"
#include "flight_controller.h"

//////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////
uint8_t ram_buf[AT45DBX_SECTOR_SIZE]; //Read and write buffer for the serial flash
ORCA_FLASH_SETTINGS_t *settings;

 /*************************************************************************
 * \brief Serial Flash initialize
 *
 * This method initialize the external serial flash 
 *
 * \param ---
 *
 * \return  true	init done
 * \return	false	error
 ***************************************************************************/
uint16_t serial_flash_init(void)
{
	/* Init serial flash */
	at45dbx_init();
	
	/* Check if the serial flash is available */
	if(at45dbx_mem_check() == true)
		return true;
		
	return false;
}

 /*************************************************************************
 * \brief Serial Flash initialize factory settings
 *
 * This method reads the settings from the external serial flash and saves 
 * the settings in the passed settings struct.\n
 * If the "Perform factory settings" was set or this is the first start with
 * the serial flash, the factory settings are written to the external flash.\n  
 *
 * \note Call this method at the beginning of the init phase to get all
 *		 saved settings.
 *
 * \param *data Settings data Struct
 *
 * \return  true	init done
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_init_factory_settings(ORCA_FLASH_SETTINGS_t *data)
 {
	 settings = data;
	 
	 /* Read the Factory Settings from the external Flash */
	 if(serial_flash_read_settings() != true)
		return false;
	 
	 /* Should we execute a factory reset? */
	 if(settings->factoryReset != SERIAL_FLASH_DISABLE_FACTORY_RESET)
	 {
		 if(serial_flash_write_factory_settings() != true)
			return false;
	 }
	 
	 return true;
 }
 
 /*************************************************************************
 * \brief Serial Flash Factory Reset
 *
 * This method setts the "Execute factory settings" bit. This will cause
 * a factory reset after the next call of serial_flash_init_factory_settings().\n
 *
 * \param --
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_factory_reset(void)
 {
	 settings->factoryReset = SERIAL_FLASH_EXECUTE_FACTORY_RESET;
		return true;
 }
 
 /*************************************************************************
 * \brief Serial Flash Read Settings
 *
 * Reads the settings from the external serial flash.\n
 *
 * \param --
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_read_settings(void)
 {
	 uint16_t strsize = sizeof(ORCA_FLASH_SETTINGS_t);
	  
	 if(strsize > AT45DBX_SECTOR_SIZE)
		return false;
		
	at45dbx_read_sector_open(SERIAL_FLASH_SETTINGS_SECTOR);
	at45dbx_read_sector_to_ram(ram_buf);
	at45dbx_read_close();
	   
	memcpy(settings,&ram_buf[0],strsize);
	
	return true;
 }
 
 /*************************************************************************
 * \brief Serial Flash Save Settings
 *
 * Saves the settings to the external serial flash.\n
 *
 * \param --
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_save_settings(void)
 {
	 /* Save actual values to settings struct */
	 
	 /* Filter settings roll */
	settings->R_angle = filter_get_roll_rangle();
	settings->Q_angle = filter_get_roll_qangle();
	settings->Q_gyro = filter_get_roll_qgyro(); 
	
	/* PID controller settings roll */
	settings->pid_roll_p_factor = flight_controller_get_pid_roll_p_factor();
	settings->pid_roll_i_factor = flight_controller_get_pid_roll_i_factor();
	settings->pid_roll_d_factor = flight_controller_get_pid_roll_d_factor();
	settings->pid_roll_i_limit = flight_controller_get_pid_roll_i_limit();
	
	/* Store data on serial flash */ 
	serial_flash_save_settings_to_flash();
 }
 
  /*************************************************************************
 * \brief Serial Flash Save Settings to Flash
 *
 * Saves the settings to the external serial flash.\n
 *
 * \param --
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_save_settings_to_flash(void)
 {
	uint16_t strsize = sizeof(ORCA_FLASH_SETTINGS_t);
		 
	if(strsize > AT45DBX_SECTOR_SIZE)
	return false;
		 
	/* Write to serial flash */
	memcpy(&ram_buf[0],settings,strsize);
	at45dbx_write_sector_open(SERIAL_FLASH_SETTINGS_SECTOR);
	at45dbx_write_sector_from_ram(ram_buf);
	at45dbx_write_close();
		 
	return true; 
 }

 /*************************************************************************
 * \brief Serial Flash Write Factory Settings
 *
 * This method sets all settings to the factory settings and saves the 
 * settings to the external serial flash.\n
 *
 * \note Add to this function all default settings, which should be stored
 *		in the external serial flash.\n
 *
 * \param --
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/	 
 uint16_t serial_flash_write_factory_settings(void)
 {
	/* No factory reset on the next MC startup */
	settings->factoryReset = SERIAL_FLASH_DISABLE_FACTORY_RESET; 
	  
	/* Set all settings to the default values */
	settings->Q_angle = FILTER_Q_ANGLE_CONF;
	settings->R_angle = FILTER_R_ANGLE_CONF;
	settings->Q_gyro = FILTER_Q_GYRO_CONF;
	settings->pid_roll_p_factor = PID_ROLL_P_FACTOR_CONF;   
	settings->pid_roll_i_factor = PID_ROLL_I_FACTOR_CONF; 
	settings->pid_roll_d_factor = PID_ROLL_D_FACTOR_CONF;
	settings->pid_roll_i_limit = PID_ROLL_I_LIMIT_CONF; 
	
	/* Write to serial flash */
	serial_flash_save_settings_to_flash();
	
	return true;
  }
     