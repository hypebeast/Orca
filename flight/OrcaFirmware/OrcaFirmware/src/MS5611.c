/*
 * MS5611.c
 *
 * Created: 18.11.2012 17:49:14
 *  Author: f-ruml
 */ 

#include "twim.h"
#include "twi_common.h"
#include "sysclk.h"
#include <delay.h>
#include "math.h"
#include "system_info.h"
#include "MS5611.h"

static uint16_t MS5611_write(uint8_t value);
static uint16_t MS5611_read(uint8_t number, uint8_t *datarec);
static void MS5611_read_all_coefficient(void);
static uint8_t crc4(uint16_t n_prom[]);
static void MS5611_prepare_adc_read(uint8_t cmd);
static void MS5611_adc_read(uint8_t cmd);
static void MS5611_calculate_p_and_t(void);

VARIOMETER_MODULET_t *ms5611;
uint16_t coefficient[8];				/*!< brief MS5611 coefficients */
uint8_t adcReadStep;
uint8_t waitTime;
unsigned long timeSinceStart;
unsigned long D1; // ADC value of the pressure conversion
unsigned long D2; // ADC value of the temperature conversion
	
void MS5611_init(VARIOMETER_MODULET_t *variometer, uint8_t res)
{
	ms5611 = variometer;
	
	MS5611_reset();
	ms5611->res = res;
	
	switch (res)
	{
		case MS5611_ADC_RES_256:
			waitTime = MS5611_WAIT_TIME_ADC_256;
		break;
		
		case MS5611_ADC_RES_512:
			waitTime = MS5611_WAIT_TIME_ADC_512;
		break;
		
		case MS5611_ADC_RES_1024:
			waitTime = MS5611_WAIT_TIME_ADC_1024;
		break;
		
		case MS5611_ADC_RES_2048:
			waitTime = MS5611_WAIT_TIME_ADC_2048;
		break;
		
		case MS5611_ADC_RES_4096:
			waitTime = MS5611_WAIT_TIME_ADC_4096;
		break;
	}
	
	/* Get the offsets */
	MS5611_read_all_coefficient();
	
	/* Calculate the CRC */
	crc4(coefficient);
	
	/* Al done! We can now read the measurements */
	adcReadStep = MS5611_READ_STEP_IDLE;
}

/**************************************************************************
* \brief MS5611 write
* Writes in a MS5611 register via the I2C Bus.
*
* \param addr the register address to be read
* \param value written to the specified register
*
* \return  status code
***************************************************************************/
static uint16_t MS5611_write(uint8_t value)
{
	twi_package_t package;
	int8_t status = 0x00;
	uint8_t data = value;
	
	package.chip = MS5611_DEV_ADDRESS;
	package.addr[0] = 0;
	package.addr[1] = 0;
	package.addr[2] = 0;
	package.addr_length = 0;
	package.buffer = &data;
	package.length = 0x01;
	package.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	status = twi_master_write(BOARD_I2C_INTERN_INTERFACE,&package);
	
	if (status != STATUS_OK)
	return SYSTEM_INFO_VARIOMETER_SENSOR_WRITE_ERR;
	
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \brief MS5611 Read
*	Reads a MS5611 register via the I2C Bus.
*
* \param addr the register address to be read
* \param number number how many bytes should be read
* \param *datarec data will be stored in this array
*
* \return  status code
***************************************************************************/
static uint16_t MS5611_read(uint8_t number, uint8_t *datarec)
{

	twi_package_t packetReceived;
	
	packetReceived.chip = MS5611_DEV_ADDRESS;
	//packetReceived.addr[0] = addr;
	packetReceived.addr_length = 0;
	packetReceived.buffer = datarec;
	packetReceived.length = number;
	packetReceived.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	twi_master_read(BOARD_I2C_INTERN_INTERFACE,&packetReceived);
	
	return SYSTEM_INFO_TRUE;
}
/**************************************************************************
* \\brief MPU 6000 Get z Gyroscope
*	Returns the z-rate of rotation in deg/s /n
*
* \\param ---
*
* \\return  z rate
***************************************************************************/
void MS5611_reset(void)
{
	/* send reset sequence */
	MS5611_write(MS5611_CMD_RESET);
	
	/* wait for the reset sequence timing */
	delay_ms(3);
}

static void MS5611_read_all_coefficient(void)
{
	uint8_t i;
	uint8_t recdata[2];
	
	for(i=0;i<=7;i++)
	{
		MS5611_write(MS5611_CMD_PROM_RD+(2*i));
		MS5611_read(2,recdata);
		coefficient[i] = (uint16_t)(recdata[0]<<8) + recdata[1];
	}
}

static uint8_t crc4(uint16_t n_prom[])
{
	uint8_t cnt; // simple counter
	uint16_t n_rem; // crc reminder
	uint16_t crc_read; // original value of the crc
	uint8_t n_bit;
	
	n_rem = 0x00;
	crc_read=n_prom[7]; //save read CRC
	n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
	
	for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
	{// choose LSB or MSB
		if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
		else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}
	n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
	n_prom[7]=crc_read; // restore the crc_read to its original place
	
	return (n_rem ^ 0x0);
}

static void MS5611_prepare_adc_read(uint8_t cmd)
{
	/* Start the conversion */
	MS5611_write(MS5611_CMD_ADC_CONV + cmd + ms5611->res);	
}

static void MS5611_adc_read(uint8_t cmd)
{
	uint8_t recdata[3];
	
	MS5611_write(MS5611_CMD_ADC_READ);
	MS5611_read(3,recdata);
	
	if(cmd == MS5611_CMD_ADC_D2)
		D2 = (unsigned long)(recdata[0]<<16) + (unsigned long)(recdata[1]<<8) + recdata[2];
	else if(cmd == MS5611_CMD_ADC_D1)
		D1 = (unsigned long)(recdata[0]<<16) + (unsigned long)(recdata[1]<<8) + recdata[2];
}

static void MS5611_calculate_p_and_t(void)
{
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature
	double T2;
	double OFF2;
	double SENS2;
		
	/* Difference between actual and reference temperature */
	dT=(double)D2-(double)coefficient[5]*pow(2,8);
	
	/* Actual temperature (-40…85°C with 0.01°C resolution) */
	ms5611->t=(2000+(dT*(double)coefficient[6])/pow(2,23))/100;
	
	/* SECOND ORDER TEMPERATURE COMPENSATION */ 
	if(ms5611->t < 20)
	{
		/* Low temperature */
		T2 = dT*dT / pow(2,31);
		OFF2 = 5*square((ms5611->t - 2000))/2;
		SENS2 = 5*square((ms5611->t - 2000))/pow(2,2);
		
		if(ms5611->t < -15)
		{
			/* Very low Temperature */
			OFF2 = OFF2 + 7*square((ms5611->t +1500));
			SENS2 = SENS2 + 11*square((ms5611->t +1500))/2;
		}
	}
	else
	{
		/* High temperature */
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;		
	}
	
	ms5611->t = ms5611->t - T2;
		
	/* Offset at actual temperature */
	OFF=coefficient[2]*pow(2,17)+dT*coefficient[4]/pow(2,6);
	OFF = OFF - OFF2;
	
	/* Sensitivity at actual temperature */
	SENS=coefficient[1]*pow(2,16)+dT*coefficient[3]/pow(2,7);
	SENS = SENS - SENS2;
	
	/* Temperature compensated pressure (10…1200mbar with 0.01mbar resolution) */
	ms5611->p=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15))/100;
}

void MS5611_read_T_P(unsigned long  time)
{
	if(adcReadStep == MS5611_READ_STEP_IDLE)
		timeSinceStart = 0;
	else
		timeSinceStart = time + timeSinceStart;
	
	switch (adcReadStep)
	{
		case MS5611_READ_STEP_IDLE:
			/* Prepare reading of D2 */
			MS5611_prepare_adc_read(MS5611_CMD_ADC_D2);
			adcReadStep = MS5611_READ_STEP_WAITING_D2;
		break;
		
		case MS5611_READ_STEP_WAITING_D2:
			if(timeSinceStart >= waitTime)
			{
				/* Reading of D2 */
				MS5611_adc_read(MS5611_CMD_ADC_D2);
				/* Prepare reading of D1 */
				MS5611_prepare_adc_read(MS5611_CMD_ADC_D1);
				adcReadStep = MS5611_READ_STEP_WAITING_D1;
			}				
		break;
		
		case MS5611_READ_STEP_WAITING_D1:
			if(timeSinceStart >= waitTime*2)
			{
				/* Reading of D2 */
				MS5611_adc_read(MS5611_CMD_ADC_D1);		
				/* Calculate P and T */
				MS5611_calculate_p_and_t();
				
				adcReadStep = MS5611_READ_STEP_IDLE;
				timeSinceStart = 0;
			}			
		break;
					
	}
}