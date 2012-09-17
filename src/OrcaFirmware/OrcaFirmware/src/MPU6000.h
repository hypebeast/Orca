/*
 * MPU6000.h
 *
 * Created: 14.09.2012 22:34:34
 *  Author: f-ruml
 */ 


#ifndef MPU6000_H_
#define MPU6000_H_

#include "user_board.h"

//---------------------------------------------------------------------
//	General
//---------------------------------------------------------------------
#define MPU_6000_I2C_SPEED				400000		/*!< brief I2C Frequenz */
#define MPU_6000_DEV_ADDRESS			0x68		/*!< brief MAX 11617 I2C Address */

//---------------------------------------------------------------------
//	MPU 6000 Startup Settings
//---------------------------------------------------------------------
#define MPU_6000_GYRO_FS_CONF			MPU_6000_GYRO_FS_250		/*!< brief full scale range of the gyroscope */
#define MPU_6000_ACC_AFS_CONF			MPU_6000_ACCEL_AFS_2g		/*!< brief full scale range of the accelerometer */

//---------------------------------------------------------------------
//	MPU Register 107 – Power Management 1, Page 44
//---------------------------------------------------------------------
#define MPU_6000_PWR_MGMT_1_ADDR		0x6B		/*!< brief MPU Register 107 – Power Management 1 */
#define MPU_6000_CLKSEL_bp				0x00		/*!< brief MPU clock source bit position */
#define MPU_6000_CLKSEL_PLL_X_GYRO		0x01		/*!< brief PLL with X axis gyroscope reference */

//---------------------------------------------------------------------
//	MPU Registers 59 to 64 – Accelerometer Measurements, Page 31
//---------------------------------------------------------------------
#define MPU_6000_ACCEL_XOUT_H		0x3B		/*!< brief MPU Register recent x-accelerometer measurements (high) */
#define MPU_6000_ACCEL_XOUT_L		0x3C		/*!< brief MPU Register recent x-accelerometer measurements (low) */
#define MPU_6000_ACCEL_YOUT_H		0x3D		/*!< brief MPU Register recent y-accelerometer measurements (high) */
#define MPU_6000_ACCEL_YOUT_L		0x3E		/*!< brief MPU Register recent y-accelerometer measurements (low) */
#define MPU_6000_ACCEL_ZOUT_H		0x3F		/*!< brief MPU Register recent z-accelerometer measurements (high) */
#define MPU_6000_ACCEL_ZOUT_L		0x40		/*!< brief MPU Register recent z-accelerometer measurements (low) */

//---------------------------------------------------------------------
//	MPU Registers 67 to 72 – Gyroscope Measurements, Page 33
//---------------------------------------------------------------------
#define MPU_6000_GYRO_XOUT_H		0x43		/*!< brief MPU Register recent x-gyroscope measurements (high) */
#define MPU_6000_GYRO_XOUT_L		0x44		/*!< brief MPU Register recent x-gyroscope measurements (low) */
#define MPU_6000_GYRO_YOUT_H		0x45		/*!< brief MPU Register recent y-gyroscope measurements (high) */
#define MPU_6000_GYRO_YOUT_L		0x46		/*!< brief MPU Register recent y-gyroscope measurements (low) */
#define MPU_6000_GYRO_ZOUT_H		0x47		/*!< brief MPU Register recent z-gyroscope measurements (high) */
#define MPU_6000_GYRO_ZOUT_L		0x48		/*!< brief MPU Register recent z-gyroscope measurements (low) */

//---------------------------------------------------------------------
//	MPU Register 28 – Accelerometer Configuration, Page 13
//---------------------------------------------------------------------
#define MPU_6000_ACCEL_CONFIG_ADDR		0x1C		/*!< brief MPU Register Accelerometer Configuration */
#define MPU_6000_ACCEL_AFS_SEL_bp		0x03		/*!< brief Full scale range of the accelerometer outputs bitposition */
#define MPU_6000_ACCEL_AFS_2g			0x00		/*!< brief ± 2g full scale range of the accelerometer (LSB Sensitivity 16384 LSB/mg) */
#define MPU_6000_ACCEL_AFS_4g			0x01		/*!< brief ± 4g full scale range of the accelerometer (LSB Sensitivity 8192 LSB/mg)*/
#define MPU_6000_ACCEL_AFS_8g			0x02		/*!< brief ± 8g full scale range of the accelerometer (LSB Sensitivity 4096 LSB/mg)*/
#define MPU_6000_ACCEL_AFS_16g			0x03		/*!< brief ± 16g full scale range of the accelerometer (LSB Sensitivity 2048 LSB/mg)*/

//---------------------------------------------------------------------
//	MPU Register 27 – Gyroscope Configuration, Page 12
//---------------------------------------------------------------------
#define MPU_6000_GYRO_CONFIG_ADDR		0x1B		/*!< brief MPU Register Gyroscope Configuration */
#define MPU_6000_GYRO_FS_SEL_bp			0x03		/*!< brief Full scale range of the gyroscope outputs bitposition */
#define MPU_6000_GYRO_FS_250			0x00		/*!< brief ± 250 °/s full scale range of the gyroscope */
#define MPU_6000_GYRO_FS_500			0x01		/*!< brief ± 500 °/s full scale range of the gyroscope */
#define MPU_6000_GYRO_FS_1000			0x02		/*!< brief ± 1000 °/s full scale range of the gyroscope */
#define MPU_6000_GYRO_FS_2000			0x03		/*!< brief ± 2000 °/s full scale range of the gyroscope */

/*! Voltage Sensor struct */
typedef struct MOTION_PROCESSING_UNIT{
	uint8_t state;								/*!< brief The actual state of the motion processing unit*/
	uint8_t accAfs;								/*!< brief Full scale range of the accelerometer */
	uint8_t gyrfs;								/*!< brief Full scale range of the gyroscope */
	int16_t xAcc;								/*!< brief x-accelerometer value */
	int16_t yAcc;								/*!< brief y-accelerometer value */
	int16_t zAcc;								/*!< brief z-accelerometer value */
	int16_t xGyr;								/*!< brief x-gyroscope value */
	int16_t yGyr;								/*!< brief y-gyroscope value */
	int16_t zGyr;								/*!< brief z-gyroscope value */
}MOTION_PROCESSING_UNIT_t;

uint16_t mpu_6000_init(MOTION_PROCESSING_UNIT_t *mpu);
uint16_t mpu_6000_write(uint8_t addr, uint8_t value);
uint16_t mpu_6000_read(uint8_t addr, uint8_t number, uint8_t *datarec);
uint16_t mpu_6000_read_accelerometer_measurements(MOTION_PROCESSING_UNIT_t *mpu);
uint16_t mpu_6000_read_gyroscope_measurements(MOTION_PROCESSING_UNIT_t *mpu);

#endif /* MPU6000_H_ */