/*
 * system_info.h
 *
 * Created: 29.05.2012 21:00:13
 *  Author: f-ruml
 */ 


#ifndef SYSTEM_INFO_H_
#define SYSTEM_INFO_H_

#ifndef nop()
#define nop()   do { __asm__ __volatile__ ("nop"); } while (0)
#endif

/*! \enum message
 * All information, error and state messages.
 */
enum system_message{
	SYSTEM_INFO_FALSE							=0x0000,	/*!< brief */
	SYSTEM_INFO_TRUE							=0x0001,	/*!< brief */
	/* Flight Controller */
	SYSTEM_INFO_WRONG_FLIGHT_MODE				=0x0010,	/*!< brief RC Controll is not possible. False mode selected. */
	SYSTEM_INFO_NO_FLIGHT_MODE					=0x0011,	/*!< brief No Flight Mode selected */
	/* Servo Input */
	SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE			=0x0020,	/*!< brief Servo Input value is inadmissible. */
	/* Voltage Sensor */
	SYSTEM_INFO_VOLTAGE_SENSOR_WRITE_ERR		=0x0030,	/*!< brief I2C Write to the DAC is not possible. */
	SYSTEM_INFO_VOLTAGE_SENSOR_NO_CH_SELECTED	=0x0031,	/*!< brief No Channel to read selected. */
	SYSTEM_INFO_VOLTAGE_SENSOR_READ_ERR			=0x0032,	/*!< brief I2C Read from the DAC is not possible. */
	SYSTEM_INFO_VOLTAGE_SENSOR_INIT_ERR			=0x0033,	/*!< brief Initaialisation of DAC is not possible. */
	SYSTEM_INFO_VOLTAGE_SENSOR_TASK_ERR			=0x0034,	/*!< brief Error during the Voltage Sensor Task. */
	/* MPU Sensor */
	SYSTEM_INFO_MPU_SENSOR_ACC_READ_ERR			=0x0040,	/*!< brief MPU acceleration I2C read error. */
	SYSTEM_INFO_MPU_SENSOR_GYRO_READ_ERR		=0x0041,	/*!< brief MPU gyro I2C read error. */
};

/*! \enum errlvl
 * Error lvl's.
 */
enum errlvl{
	ERR_LVL_LOW,	/*!< brief notice Error, keep running */
	ERR_LVL_MID,	/*!< brief notice Error, failsafe mode */
	ERR_LVL_HIGH	/*!< brief notice Error, restart! */
};

void process_info_command(enum system_message infocode,enum errlvl errorlvl);
 
#endif /* SYSTEM_INFO_H_ */