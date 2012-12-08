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

#include "system_info.h"
#include "MS5611.h"

static uint16_t MS5611_write(uint8_t value);
static uint16_t MS5611_read(uint8_t number, uint8_t *datarec);
static void MS5611_read_all_coefficient(void);
static uint8_t crc4(uint16_t n_prom[]);

VARIOMETER_MODULET_t *ms5611;
uint16_t coefficient[8];				/*!< brief MS5611 coefficients */

	
void MS5611_init(VARIOMETER_MODULET_t *variometer)
{
	ms5611 = variometer;
	
	MS5611_reset();
	
	MS5611_read_all_coefficient();
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
	//package.addr[0] = addr;
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
	int8_t status = 0x00;
	
	packetReceived.chip = MS5611_DEV_ADDRESS;
	//packetReceived.addr[0] = addr;
	packetReceived.addr_length = 0;
	packetReceived.buffer = datarec;
	packetReceived.length = number;
	packetReceived.no_wait = true;		// Don't wait, remove quickly if sending is not possible
	
	status = twi_master_read(BOARD_I2C_INTERN_INTERFACE,&packetReceived);
	
	return SYSTEM_INFO_TRUE;
}

void MS5611_reset(void)
{
	/* send reset sequence */
	MS5611_write(MS5611_CMD_RESET);
	
	/* wait for the reset sequence timing */
	delay_ms(3);
}

static void MS5611_read_all_coefficient(void)
{
	unsigned int ret;
	unsigned int rC=0;
	
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