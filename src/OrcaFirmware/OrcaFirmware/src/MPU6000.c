/*
 * MPU6000.c
 *
 * Created: 14.09.2012 22:34:20
 *  Author: f-ruml
 */


#include "twim.h"
#include "twi_common.h"
#include "sysclk.h"
#include <delay.h>

#include "MPU6000.h"
#include "system_info.h"
#include "filters.h"
uint16_t mpu_6000_read_accelerometer_measurements(void);
uint16_t mpu_6000_read_gyroscope_measurements(void);
uint16_t mpu_6000_write(uint8_t addr, uint8_t value);
uint16_t mpu_6000_read(uint8_t addr, uint8_t number, uint8_t *datarec);

MOTION_PROCESSING_UNIT_t *mpu;
PORT_t *mpuIntPort = &BOARD_MPU_6000_INT_PORT;
FILTER_DATA_t mpufilter; /*!< \brief filter module */

int16_t mpuXAcc = 0;	/*!< brief x-accelerometer value */
int16_t mpuYAcc = 0;	/*!< brief y-accelerometer value */
int16_t mpuZAcc = 0;	/*!< brief z-accelerometer value */
int16_t mpuXGyr = 0;	/*!< brief x-gyroscope value */
int16_t mpuYGyr = 0;	/*!< brief y-gyroscope value */
int16_t mpuZGyr = 0;	/*!< brief z-gyroscope value */
float accDiv = 0;
float gyrDiv = 0;

/**************************************************************************
* \\brief MPU 6000 initialization
*
* \\param *mpu MPU data structure
*
* \\return  status code
***************************************************************************/
uint16_t mpu_6000_init(MOTION_PROCESSING_UNIT_t *mProcessingUnit)
{
	mpu = mProcessingUnit;
	
	mpu->accAfs = MPU_6000_ACC_AFS_CONF;
	mpu->gyrfs = MPU_6000_GYRO_FS_CONF;
	mpu->xAcc = 0;
	mpu->yAcc = 0;
	mpu->zAcc = 0;
	mpu->xGyr = 0;
	mpu->yGyr = 0;
	mpu->zGyr = 0;
	mpu->time = 0;
	
	if (MPU_6000_ACC_AFS_CONF == MPU_6000_ACCEL_AFS_2g)
		accDiv = MPU_6000_ACCEL_AFS_FACTOR_2g;
	else if (MPU_6000_ACC_AFS_CONF == MPU_6000_ACCEL_AFS_4g)
		accDiv = MPU_6000_ACCEL_AFS_FACTOR_4g;
	else if (MPU_6000_ACC_AFS_CONF == MPU_6000_ACCEL_AFS_8g)
		accDiv = MPU_6000_ACCEL_AFS_FACTOR_8g;
	else if (MPU_6000_ACC_AFS_CONF == MPU_6000_ACCEL_AFS_16g)
		accDiv = MPU_6000_ACCEL_AFS_FACTOR_16g;
		
	if (MPU_6000_GYRO_FS_CONF == MPU_6000_GYRO_FS_250)
		gyrDiv = MPU_6000_GYRO_FS_FACTOR_250;
	else if (MPU_6000_GYRO_FS_CONF == MPU_6000_GYRO_FS_500)
		gyrDiv = MPU_6000_GYRO_FS_FACTOR_500;
	else if (MPU_6000_GYRO_FS_CONF == MPU_6000_GYRO_FS_1000)
		gyrDiv = MPU_6000_GYRO_FS_FACTOR_1000;
	else if (MPU_6000_GYRO_FS_CONF == MPU_6000_GYRO_FS_2000)
		gyrDiv = MPU_6000_GYRO_FS_FACTOR_2000;
			
	/* enable interrupt on servo input pins */
	mpuIntPort->INTCTRL = (mpuIntPort->INTCTRL & ~PORT_INT0LVL_gm) | MPU_6000_ISR_LVL;
	mpuIntPort->INT0MASK = BOARD_MPU_6000_INT_PIN_bm;
	
	/* Reset the mpu and wait some time */ 
	mpu_6000_reset();
	delay_ms(100);
	
	/* Enable and set the clock source */
	mpu_6000_write(MPU_6000_PWR_MGMT_1_ADDR, ((MPU_6000_PWR_MGMT_1_CLKSEL_PLL_X_GYRO)<<MPU_6000_PWR_MGMT_1_CLKSEL_bp)|
											((MPU_6000_PWR_MGMT_1_NO_SLEEP)<<MPU_6000_PWR_MGMT_1_SLEEP_bp)|
											((MPU_6000_PWR_MGMT_1_NO_RST)<<MPU_6000_PWR_MGMT_1_RST_bp)|
											((MPU_6000_PWR_MGMT_1_TEMP_ENABLE)<<MPU_6000_PWR_MGMT_1_TEMP_bp));
	delay_ms(1);
	
	/* Configure the sample rate divider */
	mpu_6000_write(MPU_6000_SMPRT_DIV_ADDR, MPU_6000_SMPRT_DIV_CONF);
	delay_ms(1);
	
	/* Configure DLPF for acc and gyro, disable the FSYNC Pin */
	mpu_6000_write(MPU_6000_CONFIG_ADDR, (MPU_6000_ACC_AFS_CONF)<<MPU_6000_ACCEL_AFS_SEL_bp);
	delay_ms(1);
		
	/* Configure the acceleration sensor 
		TODO: Hier fehlt noch die ueberpuefung der revision! 4g braucht bei revision C wert/4096 */
	mpu_6000_write(MPU_6000_ACCEL_CONFIG_ADDR, ((MPU_6000_CONFIG_DLPF_94)<<MPU_6000_CONFIG_DLPF_bp)| 
												((MPU_6000_CONFIG_FSYNC_DIS)<<MPU_6000_CONFIG_FSYNC_bp));
	delay_ms(1);
	
	/* Configure the gyroscope sensor */
	mpu_6000_write(MPU_6000_GYRO_CONFIG_ADDR, (MPU_6000_GYRO_FS_CONF)<<MPU_6000_GYRO_FS_SEL_bp);
	delay_ms(1);
	
	/* Init the filter module */
	filter_init(&mpufilter, FILTER_Q_ANGLE_CONF, FILTER_Q_GYRO_CONF, FILTER_R_ANGLE_CONF);
	
	return SYSTEM_INFO_TRUE;
}

uint16_t mpu_6000_write(uint8_t addr, uint8_t value)
{
	twi_package_t package;
	int8_t status = 0x00;
	uint8_t data = value;
	
	package.chip = MPU_6000_DEV_ADDRESS;
	package.addr[0] = addr;
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
	int8_t status = 0x00;
		
	packetReceived.chip = MPU_6000_DEV_ADDRESS;
	packetReceived.addr[0] = addr;
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
uint16_t mpu_6000_read_accelerometer_measurements(void)
{
	uint8_t accReceived[6];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	if(mpu_6000_read(MPU_6000_ACCEL_XOUT_H, 6, accReceived) != SYSTEM_INFO_TRUE)
		return SYSTEM_INFO_MPU_SENSOR_ACC_READ_ERR;
	
	/* Built the 16-bit 2’s complement value */	
	mpuXAcc = (int16_t)(accReceived[0]<<8) + accReceived[1];
	mpuYAcc = (int16_t)(accReceived[2]<<8) + accReceived[3];
	mpuZAcc = (int16_t)(accReceived[4]<<8) + accReceived[5];
	 
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
uint16_t mpu_6000_read_gyroscope_measurements(void)
{
	uint8_t gyroReceived[6];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	if(mpu_6000_read(MPU_6000_GYRO_XOUT_H, 6, gyroReceived) != SYSTEM_INFO_TRUE)
		return SYSTEM_INFO_MPU_SENSOR_GYRO_READ_ERR;
	
	/* Built the 16-bit 2’s complement value */
	mpuXGyr = gyroReceived[1] + (gyroReceived[0]<<8);
	mpuYGyr = gyroReceived[3] + (gyroReceived[2]<<8);
	mpuZGyr = gyroReceived[5] + (gyroReceived[4]<<8);
	
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \\brief MPU 6000 is available
*	Chekcs the MPU 6000 on the I2C bus and returns true if the sensor is
*	available . /n
*
* \\param *mpu MPU data structure
*
* \\return  true, device is accessible
			false, device is not responding
***************************************************************************/
uint16_t mpu_6000_is_available(void)
{
	
	//TODO: Noch verfollstaendigen!
	uint8_t mpurecdata[1];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	if(mpu_6000_read(MPU_6000_WHO_AM_I_ADDR, 1, mpurecdata) != SYSTEM_INFO_TRUE)
		return SYSTEM_INFO_MPU_SENSOR_GYRO_READ_ERR;
	
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \\brief MPU 6000 is available
*	Chekcs the MPU 6000 on the I2C bus and returns true if the sensor is
*	available . /n
*
* \\param *mpu MPU data structure
*
* \\return  true, device is accessible
			false, device is not responding
***************************************************************************/
int16_t mpu_6000_get_z_acc_offset(void)
{
	uint8_t mpurecdata[2];
	
	/* Read all 3 acceleration axis. The value of each axis is read as a 16-bit 2’s complement value. */
	mpu_6000_read(0x0A, 2, mpurecdata);

	return ((int16_t)(mpurecdata[0]<<8) + mpurecdata[1]);
}

/**************************************************************************
* \\brief MPU 6000 get product id
*	Returns the mpu product id . /n
*
* \\param *mpu MPU data structure
*
* \\return  product id			
***************************************************************************/
uint8_t mpu_6000_get_product_id(void)
{
	uint8_t mpurecdata[1];
	
	/* Read the product id */
	mpu_6000_read(MPU_6000_PRODUCT_ID_ADDR, 1, mpurecdata);

	return mpurecdata[0];
}

/**************************************************************************
* \\brief MPU 6000 reset
*	Reset the MPU . /n
*
* \\param *mpu MPU data structure
*
* \\return  true, reset done
			false, write error			
***************************************************************************/
uint16_t mpu_6000_reset(void)
{
	if (mpu_6000_write(MPU_6000_PWR_MGMT_1_ADDR, (MPU_6000_PWR_MGMT_1_RST)<<MPU_6000_PWR_MGMT_1_RST_bp) != SYSTEM_INFO_TRUE)
		return SYSTEM_INFO_MPU_SENSOR_ACC_WRITE_ERR;
		
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \\brief MPU 6000 get new data
*	Get the acceleration and rate of rotation measurements from the mpu,
*   convert and save the new value in the mpu struct . /n
*
* \\param *mpu MPU data structure
*
* \\return  product id
***************************************************************************/
uint8_t mpu_6000_get_new_data(void)
{
	/* Read all Acc and Gyro axis */
	mpu_6000_read_accelerometer_measurements();
	mpu_6000_read_gyroscope_measurements();
	
	/* Scaling and save all measurements */
	mpu->xAcc = (float)mpuXAcc / accDiv; //Save the X-Acceleration in g
	mpu->yAcc = (float)mpuYAcc / accDiv; //Save the Y-Acceleration in g
	mpu->zAcc = (float)mpuZAcc / accDiv; //Save the Z-Acceleration in g
	mpu->xGyr = (float)mpuXGyr / gyrDiv; //Save the X-rate of rotation in deg/s
	mpu->yGyr = (float)mpuYGyr / gyrDiv; //Save the Y-rate of rotation in deg/s
	mpu->zGyr = (float)mpuZGyr / gyrDiv; //Save the Z-rate of rotation in deg/s
	
	return true;
}
/**************************************************************************
* \\brief MPU 6000 get new data
*	Get the acceleration and rate of rotation measurements from the mpu,
*   convert and save the new value in the mpu struct . /n
*
* \\param *mpu MPU data structure
*
* \\return  product id
***************************************************************************/
uint8_t mpu_6000_task(void)
{
	mpu->time += 10000;
	mpu_6000_get_new_data();
	
	mpufilter.xAcc = -(mpu->xAcc);
	mpufilter.yAcc = mpu->yAcc;
	mpufilter.zAcc = -(mpu->zAcc);
	mpufilter.xGyr = -(mpu->xGyr);
	mpufilter.yGyr = mpu->yGyr;
	mpufilter.zGyr = mpu->zGyr;
	
	filter_task(mpu->time);
}

void isr_mpu_6000_int_pin(void)
{
	
}	
