/*
 * voltage_sens.c
 *
 * Created: 20.06.2012 13:15:07
 *  Author: f-ruml
 */ 

//#include <twim.h>
//#include <twi_common.h>
//#include <sysclk.h>

#include <asf.h>

#include "system_info.h"
#include "voltage_sens.h"


uint16_t voltage_sens_init(VOLTAGE_SENSOR_t *sensor, uint8_t chNumber)
{
//	twi_options_t opt;
	
	sensor->channelNumber = chNumber;
	sensor->state = VOLTAGE_SENS_STATE_ERR;
	sensor->ai0 = 0x00;
	sensor->ai1 = 0x00;
	sensor->voltage0 = 0x00;
	sensor->voltage1 = 0x00;
		
//	opt.speed = VOLTAGE_SENS_I2C_SPEED;
//	opt.chip = VOLTAGE_SENS_DEV_ADDRESS;
//	opt.speed_reg = TWI_BAUD(32000000,VOLTAGE_SENS_I2C_SPEED);
	
//	sysclk_enable_peripheral_clock(BOARD_I2C_INTERN_INTERFACE);
//	
//	/* Init TWI interface */
//	if(twi_master_init(BOARD_I2C_INTERN_INTERFACE, &opt) != STATUS_OK)
//		return SYSTEM_INFO_VOLTAGE_SENSOR_INIT_ERR;

	twi_master_enable(BOARD_I2C_INTERN_INTERFACE);
		
	/*	Write ADC Setup Settings:
	* - Unipolar measurement
	* - Internal clock
	* - Internal reference voltage
	*/
	//if(voltage_sens_write_setup(VOLTAGE_SENS_NO_RST|VOLTAGE_SENS_UNIPOLAR|VOLTAGE_SENS_INT_CLK|VOLTAGE_SENS_REF_VOLT_INT_4) != SYSTEM_INFO_TRUE)
		//return SYSTEM_INFO_VOLTAGE_SENSOR_INIT_ERR;
	//VOLTAGE_SENS_REF_VOLT_INT_1
		
	/*	Write ADC Config Settings:
	* - Scans up from AIN0 to AIN1 
	* - Single Ended Mode
	*/
	//if (voltage_sens_write_config(VOLTAGE_SENS_SCAN_TO_SELECT|VOLTAGE_SENS_CS_AIN1|VOLTAGE_SENS_SGL) != SYSTEM_INFO_TRUE)
		//return SYSTEM_INFO_VOLTAGE_SENSOR_INIT_ERR;
		
	sensor->state = VOLTAGE_SENS_STATE_OK;
	
	return SYSTEM_INFO_TRUE;
}

uint16_t voltage_sens_write_setup(uint8_t setup)
{
	twi_package_t package;
	uint8_t data = VOLTAGE_SENS_SETUP_CMD;
	int8_t status = 0x00; 
	
	package.chip = VOLTAGE_SENS_DEV_ADDRESS;
	package.addr[0] = 0;
	package.addr_length = 0;
	package.buffer = &data;
	package.length = 0x01;
	package.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	data |= setup ;
	  
	status = twi_master_write(BOARD_I2C_INTERN_INTERFACE,&package);
	
	if (status != STATUS_OK)
		return SYSTEM_INFO_VOLTAGE_SENSOR_WRITE_ERR;
		
	return SYSTEM_INFO_TRUE;
}

//uint16_t voltage_sens_write_config(uint8_t conf)
//{
	//twi_package_t package;
	//uint8_t data = VOLTAGE_SENS_CONFIG_CMD;
	//int8_t status = 0x00; 
	//
	//package.chip = VOLTAGE_SENS_DEV_ADDRESS;
	//package.addr[0] = 0;
	//package.addr_length = 0;
	//package.buffer = &data;
	//package.length = 0x01;
	//package.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	//
	//data |= conf ;
	//
	//status = twi_master_write(BOARD_I2C_INTERN_INTERFACE,&package);
	//
	//if (status != STATUS_OK)
		//return SYSTEM_INFO_VOLTAGE_SENSOR_WRITE_ERR;
		//
	//return SYSTEM_INFO_TRUE;
//}

//uint16_t voltage_sens_reset_conf(void)
//{
	//return voltage_sens_write_setup(VOLTAGE_SENS_RST);
//}

//uint16_t voltage_sens_read_channels(VOLTAGE_SENSOR_t *sensor)
//{
	//if(sensor->channelNumber <= 0x00)
		//return SYSTEM_INFO_VOLTAGE_SENSOR_NO_CH_SELECTED;
		//
	//twi_package_t packetReceived;
	////uint8_t dataReceived[(sensor->channelNumber)*2];	// One Channel is stored in two Bytes
	//uint8_t dataReceived[4];	// One Channel is stored in two Bytes
	//int8_t status = 0x00;
	////uint8_t i = 0x00;
	//
	//packetReceived.chip = VOLTAGE_SENS_DEV_ADDRESS;
	//packetReceived.addr[0] = 0;
	//packetReceived.addr_length = 0;
	//packetReceived.buffer = dataReceived;
	//packetReceived.length = (sensor->channelNumber)*2;	// One Channel is stored in two Bytes
	//packetReceived.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	//
	//status = twi_master_read(BOARD_I2C_INTERN_INTERFACE,&packetReceived);
	//
	//if (status != STATUS_OK)
	//return SYSTEM_INFO_VOLTAGE_SENSOR_READ_ERR;
	//
	///* Convert the result to uint16 */
	//
////	for (i=0; i<=(sensor->channelNumber)-1; i++)
////	{
////		
////	}
	//
	//sensor->ai0 = ((dataReceived[0] & 0x0F)<<0x08)+dataReceived[1];
	//sensor->ai1 = ((dataReceived[2] & 0x0F)<<0x08)+dataReceived[3];
	//
	//return SYSTEM_INFO_TRUE;
//}

//uint16_t voltage_sens_task(VOLTAGE_SENSOR_t *sensor)
//{
//
	//if(voltage_sens_read_channels(sensor) != SYSTEM_INFO_TRUE) 
		//return SYSTEM_INFO_VOLTAGE_SENSOR_TASK_ERR;
//
	//sensor->voltage0 = (uint16_t) (sensor->ai0 * VOLTAGE_SENS_AI0_SCALE)+VOLTAGE_SENS_DIODE_OFFSET;
	//sensor->voltage1 = (uint16_t) (sensor->ai1 * VOLTAGE_SENS_AI1_SCALE);
//
	//return SYSTEM_INFO_TRUE;
//}
