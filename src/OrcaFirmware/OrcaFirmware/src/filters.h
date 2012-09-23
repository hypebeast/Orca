/*
 * filters.h
 *
 * Created: 23.09.2012 15:23:56
 *  Author: f-ruml
 */ 


#ifndef FILTERS_H_
#define FILTERS_H_

#define PI 3.14159265358979f

#define FILTERS_GYRO_FACTOR_CONF					10

/*! MPU6000 sensor struct */
typedef struct FILTER_DATA{
	uint8_t firstSample;						/*!< brief The actual state of the motion processing unit*/	
	float gyroFactor;							/*!< brief Gyro factor / weight for the calculations */
	
	float axr;
	float ayr;
	float azr;
}FILTER_DATA_t;

void filters_init(FILTER_DATA_t *filterData);
void filters_get_estimated_inclination(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr);

#endif /* FILTERS_H_ */