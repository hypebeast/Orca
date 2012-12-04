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
#define MS5611_DEV_ADDRESS			0xEE		/*!< brief MS5611 I2C Address */


//---------------------------------------------------------------------
//	MS5611 Register Address
//---------------------------------------------------------------------
#define MS5611_CMD_RESET			0x1E		/*!< brief  ADC reset command */
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



/*! MS5611 sensor struct */
typedef struct VARIOMETER_MODULE{
	uint8_t state;				/*!< brief The actual state of the motion processing unit */
		
}VARIOMETER_MODULET_t;

void MS5611_init(VARIOMETER_MODULET_t *variometer);
void MS5611_reset();
#endif /* MS5611_H_ */