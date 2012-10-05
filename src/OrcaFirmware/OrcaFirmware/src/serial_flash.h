/*
 * serial_flash.h
 *
 * Created: 05.10.2012 15:14:14
 *  Author: f-ruml
 */ 


#ifndef SERIAL_FLASH_H_
#define SERIAL_FLASH_H_

#define SERIAL_FLASH_EXECUTE_FACTORY_RESET			0xff		/*!< \brief After the next restart a factory reset will be executed */
#define SERIAL_FLASH_DISABLE_FACTORY_RESET			0x00		/*!< \brief After the next restart no factory reset will be executed */


/*! Orca flash settings struct
 *  Holds all Orca settings */
typedef struct ORCA_FLASH_SETTINGS
{
	uint8_t factoryReset;					/*!< \brief If != 0x00, will cause an factory reset */
	/* Filter Settings */
	float Q_angle;							/*!< \brief  */
} ORCA_FLASH_SETTINGS_t;


uint16_t serial_flash_init(void);
uint16_t serial_flash_init_factory_settings(ORCA_FLASH_SETTINGS_t *data);
uint16_t serial_flash_factory_reset();
uint16_t serial_flash_read_factory_settings();
uint16_t serial_flash_write_factory_settings();

#endif /* SERIAL_FLASH_H_ */