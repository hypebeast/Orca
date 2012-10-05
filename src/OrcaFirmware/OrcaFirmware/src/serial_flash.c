/*
 * serial_flash.c
 *
 * Created: 05.10.2012 15:13:58
 *  Author: f-ruml
 */ 

#include "sysclk.h"
#include "string.h"
#include "at45dbx.h"
#include "serial_flash.h"
#include "filters.h"

uint8_t ram_buf[AT45DBX_SECTOR_SIZE];
ORCA_FLASH_SETTINGS_t *settings;

/**************************************************************************
* \\brief MPU 6000 initialization
*
* \\param *mpu MPU data structure
*
* \\return  status code
***************************************************************************/
uint16_t serial_flash_init()
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
 * \param *flashData PLC-Settings data Struct
 *
 * \return  true	all right
 * \return	false	error
 ***************************************************************************/
 uint16_t serial_flash_init_factory_settings(ORCA_FLASH_SETTINGS_t *data)
 {
	 settings = data;
	 
	 /* Read the Factory Settings from the external Flash */
	 if(serial_flash_read_factory_settings() != true)
		return false;
	 
	 /* Should we execute a factory reset */
	 if(settings->factoryReset != SERIAL_FLASH_DISABLE_FACTORY_RESET)
	 {
		 if(serial_flash_factory_reset() != true)
			return false;
	 }
	 
	 return true;
 }
 
 uint16_t serial_flash_factory_reset()
 {
	 
 }
 
 uint16_t serial_flash_read_factory_settings()
 {
	   at45dbx_read_sector_open(0x00002);
	   at45dbx_read_sector_to_ram(ram_buf);
	   at45dbx_read_close();
	   
	   memcpy(settings,&ram_buf[0],sizeof(ORCA_FLASH_SETTINGS_t));
 }
  
  uint16_t serial_flash_write_factory_settings()
  {
	   settings->Q_angle = FILTER_Q_ANGLE_CONF;
	   
	   memcpy(&ram_buf[0],settings,sizeof(ORCA_FLASH_SETTINGS_t));
	   at45dbx_write_sector_open(0x00002);
	   at45dbx_write_sector_from_ram(ram_buf);
	   at45dbx_write_close();
  }
     