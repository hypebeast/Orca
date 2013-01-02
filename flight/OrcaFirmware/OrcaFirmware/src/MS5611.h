/*
 * MS5611.h
 *
 * Created: 18.11.2012 17:49:29
 *  Author: f-ruml
 */ 


#ifndef MS5611_H_
#define MS5611_H_

#include "user_board.h"

//---------------------------------------------------------------------
//	General
//---------------------------------------------------------------------
#define MS5611_DEV_ADDRESS			0x77		/*!< brief MS5611 I2C Address */

//---------------------------------------------------------------------
//	ADC Read Steps
//---------------------------------------------------------------------
#define MS5611_READ_STEP_IDLE		0x00		/*!< brief Read step idle */
#define MS5611_READ_STEP_WAITING_D2	0x01		/*!< brief Read step waiting for conversion time */
#define MS5611_READ_STEP_WAITING_D1	0x02		/*!< brief Read step waiting for conversion time */
#define MS5611_READ_STEP_DATA_READY	0x03		/*!< brief Read step data ready */

//---------------------------------------------------------------------
//	ADC Resolution
//---------------------------------------------------------------------
#define MS5611_ADC_RES_256			MS5611_CMD_ADC_256
#define MS5611_ADC_RES_512			MS5611_CMD_ADC_512
#define MS5611_ADC_RES_1024			MS5611_CMD_ADC_1024
#define MS5611_ADC_RES_2048			MS5611_CMD_ADC_2048
#define MS5611_ADC_RES_4096			MS5611_CMD_ADC_4096

//---------------------------------------------------------------------
//	ADC Wait Time (in ms)
//---------------------------------------------------------------------
#define MS5611_WAIT_TIME_ADC_256		1		/*!< brief The conversion wait time for 256 resolution in ms */
#define MS5611_WAIT_TIME_ADC_512		3		/*!< brief The conversion wait time for 512 resolution in ms */
#define MS5611_WAIT_TIME_ADC_1024		4		/*!< brief The conversion wait time for 1024 resolution in ms */
#define MS5611_WAIT_TIME_ADC_2048		6		/*!< brief The conversion wait time for 2048 resolution in ms */
#define MS5611_WAIT_TIME_ADC_4096		10		/*!< brief The conversion wait time for 4096 resolution in ms */

//---------------------------------------------------------------------
//	MS5611 Register Address
//---------------------------------------------------------------------
#define MS5611_CMD_RESET			0x1E		/*!< brief ADC reset command */
#define MS5611_CMD_ADC_READ			0x00		/*!< brief ADC read command */
#define MS5611_CMD_ADC_CONV			0x40		/*!< brief ADC conversion command */
#define MS5611_CMD_ADC_D1			0x00		/*!< brief ADC D1 conversion */
#define MS5611_CMD_ADC_D2			0x10		/*!< brief ADC D2 conversion */
#define MS5611_CMD_ADC_256			0x00		/*!< brief ADC OSR=256 */
#define MS5611_CMD_ADC_512			0x02		/*!< brief ADC OSR=512 */
#define MS5611_CMD_ADC_1024			0x04		/*!< brief ADC OSR=1024 */
#define MS5611_CMD_ADC_2048			0x06		/*!< brief ADC OSR=2048 */
#define MS5611_CMD_ADC_4096			0x08		/*!< brief ADC OSR=4096 */
#define MS5611_CMD_PROM_RD			0xA0		/*!< brief Prom read command */

#define MS5611_P_REF				1013.25		/*!< brief MSL reference pressure at sea height in hpa(mbar) */

/*! MS5611 sensor struct */
typedef struct AIR_PRESSURE_SENSOR{
	uint8_t res;
	double p;					/*!< brief compensated pressure value in mbar */
	double t;					/*!< brief compensated temperature value in °C */
	float altitudeRef;			/*!< brief Referenze altitude for the relative measurement in m*/
	float absoluteAltitude;		/*!< brief Absolute altitude value in m */
	float relativeAltitude;		/*!< brief Relative altitude value in m */
}AIR_PRESSURE_SENSOR_t;

void MS5611_init(AIR_PRESSURE_SENSOR_t *airPressureSensor, uint8_t res);
void MS5611_reset(void);
uint8_t MS5611_read_t_p(unsigned long  time);
uint8_t MS5611_altimeter_task(unsigned long  time);
void MS5611_set_reference_altitude(void);
float MS5611_get_reference_altitude(void);
float MS5611_get_absolute_altitude(void);
float MS5611_get_relative_altitude(void);
float MS5611_get_temperature(void);
float MS5611_get_pressure(void);
#endif /* MS5611_H_ */