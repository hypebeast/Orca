/*
 * MPU6000.c
 *
 * Created: 14.09.2012 22:34:20
 *  Author: f-ruml
 */ 
#include "twim.h"
#include "twi_common.h"
#include "sysclk.h"

#include "MPU6000.h"
#include "system_info.h"

uint16_t mpu_6000_init(MOTION_PROCESSING_UNIT_t *mpu)
{
	mpu->xAcc = 0;
	mpu->yAcc = 0;
	mpu->zAcc = 0;
	mpu->xGyr = 0;
	mpu->yGyr = 0;
	mpu->zGyr = 0;
	
	mpu->accAfs = MPU_6000_ACC_AFS_CONF;
	mpu->gyrfs = MPU_6000_GYRO_FS_CONF;
	
	mpu_6000_write(MPU_6000_PWR_MGMT_1_ADDR, (MPU_6000_CLKSEL_PLL_X_GYRO)<<MPU_6000_CLKSEL_bp);
	
	/* Configure the acceleration sensor */
	mpu_6000_write(MPU_6000_ACCEL_CONFIG_ADDR, (MPU_6000_ACC_AFS_CONF)<<MPU_6000_ACCEL_AFS_SEL_bp);
	
	/* Configure the gyroscope sensor */
	mpu_6000_write(MPU_6000_GYRO_CONFIG_ADDR, (MPU_6000_GYRO_FS_CONF)<<MPU_6000_GYRO_FS_SEL_bp);
	
}

uint16_t mpu_6000_write(uint8_t addr, uint8_t value)
{
	twi_package_t package;
	int8_t status = 0x00;
	uint8_t data = value;
	
	package.chip = MPU_6000_DEV_ADDRESS;
	package.addr = addr;
	package.addr_length = 1;
	package.buffer = &data;
	package.length = 0x01;
	package.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	status = twi_master_write(BOARD_I2C_INTERN_INTERFACE,&package);
	
	if (status != STATUS_OK)
	return SYSTEM_INFO_VOLTAGE_SENSOR_WRITE_ERR;
	
	return SYSTEM_INFO_TRUE;
}

uint16_t mpu_6000_read(uint8_t addr, uint8_t number, uint8_t *datarec)
{

	twi_package_t packetReceived;
	//uint8_t dataReceived[number];	// One Channel is stored in two Bytes
	int8_t status = 0x00;
	uint8_t i = 0x00;
		
	packetReceived.chip = MPU_6000_DEV_ADDRESS;
	packetReceived.addr = addr;
	packetReceived.addr_length = 1;
	packetReceived.buffer = datarec;
	packetReceived.length = number;	
	packetReceived.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	status = twi_master_read(BOARD_I2C_INTERN_INTERFACE,&packetReceived);
		
	return SYSTEM_INFO_TRUE;
}	

/**************************************************************************
* \\brief MPU 6000 Read Accelerometer Measurements
*	Reads the accelerometer measurements (x,y and z-axis). /n
*	The values are saved in the MPU data structure. /n
*
* \\param *mpu MPU data structure
*
* \\return  status code
***************************************************************************/
uint16_t mpu_6000_read_accelerometer_measurements(MOTION_PROCESSING_UNIT_t *mpu)
{
	uint8_t accReceived[6];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	if(mpu_6000_read(MPU_6000_ACCEL_XOUT_H, 6, accReceived) != SYSTEM_INFO_TRUE)
		return SYSTEM_INFO_MPU_SENSOR_ACC_READ_ERR;
	
	/* Built the 16-bit 2’s complement value */	
	mpu->xAcc = accReceived[1] + (accReceived[0]<<8);
	mpu->yAcc = accReceived[3] + (accReceived[2]<<8);
	mpu->zAcc = accReceived[5] + (accReceived[4]<<8);
	 
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \\brief MPU 6000 Read Gyroscope Measurements
*	Reads the gyroscope measurements (x,y and z-axis). /n
*	The values are saved in the MPU data structure. /n
*
* \\param *mpu MPU data structure
*
* \\return  status code
***************************************************************************/
uint16_t mpu_6000_read_gyroscope_measurements(MOTION_PROCESSING_UNIT_t *mpu)
{
	uint8_t gyroReceived[6];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	if(mpu_6000_read(MPU_6000_GYRO_XOUT_H, 6, gyroReceived) != SYSTEM_INFO_TRUE)
	return SYSTEM_INFO_MPU_SENSOR_GYRO_READ_ERR;
	
	/* Built the 16-bit 2’s complement value */
	mpu->xGyr = gyroReceived[1] + (gyroReceived[0]<<8);
	mpu->yGyr = gyroReceived[3] + (gyroReceived[2]<<8);
	mpu->zGyr = gyroReceived[5] + (gyroReceived[4]<<8);
	
	return SYSTEM_INFO_TRUE;
}