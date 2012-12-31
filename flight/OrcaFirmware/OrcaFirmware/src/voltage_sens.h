/*
 * voltage_sens.h
 *
 * Created: 20.06.2012 13:15:24
 *  Author: f-ruml
 */ 


#ifndef VOLTAGE_SENS_H_
#define VOLTAGE_SENS_H_

#include "user_board.h"


//---------------------------------------------------------------------
//	General 
//---------------------------------------------------------------------
#define VOLTAGE_SENS_I2C_SPEED				400000		/*!< brief I2C Frequenz */	
#define VOLTAGE_SENS_DEV_ADDRESS			0x35		/*!< brief MAX 11617 I2C Address */

#define VOLTAGE_SENS_STATE_OK				0x01
#define VOLTAGE_SENS_STATE_ERR				0x00

#define VOLTAGE_SENS_INT_REF_V				2048		/*!< brief Internal DAC Ref Voltage in mV */
#define VOLTAGE_SENS_RESOLUTION				4096		/*!< brief DAC Resolution 12 Bit */
#define VOLTAGE_SENS_DIFIDER				(VOLTAGE_SENS_INT_REF_V/VOLTAGE_SENS_RESOLUTION)			
//#define VOLZAGE_SENS_AI0_SCALE				(float)(VOLTAGE_SENS_DIFIDER*(100/20))
#define VOLTAGE_SENS_AI0_SCALE				3.0f
#define VOLTAGE_SENS_AI1_SCALE				(VOLTAGE_SENS_DIFIDER*(100/10))
#define VOLTAGE_SENS_DIODE_OFFSET			150			/*!< brief Diode Offset in mV */
//---------------------------------------------------------------------
//	Setup Byte Format, Page 13
//---------------------------------------------------------------------
#define VOLTAGE_SENS_SETUP_CMD_bp			0x07
#define VOLTAGE_SENS_SETUP_CMD				((0x01)<<VOLTAGE_SENS_SETUP_CMD_bp)
#define VOLTAGE_SENS_SEL0_bp				0x04
#define VOLTAGE_SENS_SEL1_bp				0x05
#define VOLTAGE_SENS_SEL2_bp				0x06
#define VOLTAGE_SENS_CLK_bp					0x03
#define VOLTAGE_SENS_INT_CLK				((0x00)<<VOLTAGE_SENS_CLK_bp)
#define VOLTAGE_SENS_EXT_CLK				((0x01)<<VOLTAGE_SENS_CLK_bp)
#define VOLTAGE_SENS_BIP_bp					0x02
#define VOLTAGE_SENS_BIPOLAR				((0x01)<<VOLTAGE_SENS_BIP_bp)
#define VOLTAGE_SENS_UNIPOLAR				((0x00)<<VOLTAGE_SENS_BIP_bp)
#define VOLTAGE_SENS_RST_bp					0x01
#define VOLTAGE_SENS_NO_RST					((0x01)<<VOLTAGE_SENS_RST_bp)
#define VOLTAGE_SENS_RST					((0x00)<<VOLTAGE_SENS_RST_bp)

//---------------------------------------------------------------------
//	Config Byte Format, Page 14
//---------------------------------------------------------------------
#define VOLTAGE_SENS_CONFIG_CMD_bp			0x07									/*!< brief Config CMD Bit positon */
#define VOLTAGE_SENS_CONFIG_CMD				((0x00)<<VOLTAGE_SENS_CONFIG_CMD_bp)	/*!< brief Config CMD */
#define VOLTAGE_SENS_SCAN1_bp				0x06									/*!< brief Scan Select Bit 1 Bit position */
#define VOLTAGE_SENS_SCAN0_bp				0x05									/*!< brief Scan Select Bit 0 Bit position */
#define VOLTAGE_SENS_CS3_bp					0x04									/*!< brief Channel Select Bit 3 Bit position */
#define VOLTAGE_SENS_CS2_bp					0x03									/*!< brief Channel Select Bit 2 Bit position */
#define VOLTAGE_SENS_CS1_bp					0x02									/*!< brief Channel Select Bit 1 Bit position */
#define VOLTAGE_SENS_CS0_bp					0x01									/*!< brief Channel Select Bit 0 Bit position */
#define VOLTAGE_SENS_SGL_bp					0x00									/*!< brief Single Ended/Differential Input Bit position */
#define VOLTAGE_SENS_SGL					((0x01)<<VOLTAGE_SENS_SGL_bp)			/*!< brief Single Ended Mode */
#define VOLTAGE_SENS_DIF					((0x00)<<VOLTAGE_SENS_SGL_bp)			/*!< brief Differential Mode */

//---------------------------------------------------------------------
//	Reference Voltage, Data sheet Page 18
//---------------------------------------------------------------------
#define VOLTAGE_SENS_REF_VOLT_VDD			((0x00)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief Reference voltage VDD */
#define VOLTAGE_SENS_REF_VOLT_EXT			((0x02)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief External reference Voltage */
#define VOLTAGE_SENS_REF_VOLT_INT_1			((0x04)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief Internal reference Voltage, Analog input, State always off, 2,048 V*/
#define VOLTAGE_SENS_REF_VOLT_INT_2			((0x05)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief Internal reference Voltage, Analog input, State always on */
#define VOLTAGE_SENS_REF_VOLT_INT_3			((0x06)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief Internal reference Voltage, Reference Output, State always off */
#define VOLTAGE_SENS_REF_VOLT_INT_4			((0x07)<<VOLTAGE_SENS_SEL0_bp)		/*!< brief Internal reference Voltage, Reference Output, State always on */


//---------------------------------------------------------------------
//	Channel selection in Sengle-Ended Mode, Data sheet Page 14
//---------------------------------------------------------------------
#define VOLTAGE_SENS_CS_AIN0			((0x00)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN0 */
#define VOLTAGE_SENS_CS_AIN1			((0x01)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN1 */
#define VOLTAGE_SENS_CS_AIN2			((0x02)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN2 */
#define VOLTAGE_SENS_CS_AIN3			((0x03)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN3 */
#define VOLTAGE_SENS_CS_AIN4			((0x04)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN4 */
#define VOLTAGE_SENS_CS_AIN5			((0x05)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN5 */
#define VOLTAGE_SENS_CS_AIN6			((0x06)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN6 */
#define VOLTAGE_SENS_CS_AIN7			((0x07)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN7 */
#define VOLTAGE_SENS_CS_AIN8			((0x08)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN8 */
#define VOLTAGE_SENS_CS_AIN9			((0x09)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN9 */
#define VOLTAGE_SENS_CS_AIN10			((0x0A)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN10 */
#define VOLTAGE_SENS_CS_AIN11			((0x0B)<<VOLTAGE_SENS_CS0_bp)		/*!< brief Select AIN11 */

//---------------------------------------------------------------------
//	Scanning Configuration, Data sheet Page 17
//---------------------------------------------------------------------
#define VOLTAGE_SENS_SCAN_TO_SELECT		((0x00)<<VOLTAGE_SENS_SCAN0_bp)		/*!< brief Scans up from AIN0 to the input selected by CS3–CS0 */
#define VOLTAGE_SENS_SCAN_8_TIMES		((0x01)<<VOLTAGE_SENS_SCAN0_bp)		/*!< brief Converts the input selected by CS3–CS0 eight times */
#define VOLTAGE_SENS_SCAN_UPPER_HALF	((0x02)<<VOLTAGE_SENS_SCAN0_bp)		/*!< brief Scans up from AIN6 to the input selected by CS3–CS0. */
#define VOLTAGE_SENS_SCAN_SELECT		((0x03)<<VOLTAGE_SENS_SCAN0_bp)		/*!< brief Converts channel selected by CS3–CS0. */


/*! Voltage Sensor struct */
typedef struct VOLTAGE_SENSOR {
	uint8_t state;
	uint8_t channelNumber;
	volatile uint16_t ai0;						/*!< brief DAC Input 0 value */
	volatile uint16_t ai1;						/*!< brief DAC Input 01 value */
	uint16_t voltage0;							/*!< brief Voltage 0 in mV */
	uint16_t voltage1;							/*!< brief Voltage 1 in mV */
}VOLTAGE_SENSOR_t;

uint16_t voltage_sens_write_Setup(uint8_t setup);
uint16_t voltage_sens_init(VOLTAGE_SENSOR_t *sensor, uint8_t chNumber);
uint16_t voltage_sens_write_config(uint8_t conf);
uint16_t voltage_sens_reset_conf(void);
uint16_t voltage_sens_read_channels(VOLTAGE_SENSOR_t *sensor);
uint16_t voltage_sens_task(VOLTAGE_SENSOR_t *sensor);

#endif /* VOLTAGE_SENS_H_ */