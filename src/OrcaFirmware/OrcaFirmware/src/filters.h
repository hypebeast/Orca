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
#define ACC_WEIGHT_MAX 0.02                     //maximum accelerometer weight in accelerometer-gyro fusion formula
                                                //this value is tuned-up experimentally: if you get too much noise - decrease it
                                                //if you get a delayed response of the filtered values - increase it
                                                //starting with a value of  0.01 .. 0.05 will work for most sensors

#define ACC_ERR_MAX  0.3        //maximum allowable error(external acceleration) where accWeight becomes 0
#define ACC_WEIGHT 0.01         //accelerometer data weight relative to gyro's weight of 1
#define MAG_WEIGHT 0.0          //magnetometer data weight relative to gyro's weight of 1

/*! MPU6000 sensor struct */
typedef struct FILTER_DATA{
	uint8_t firstSample;						/*!< brief The actual state of the motion processing unit*/	
	float gyroFactor;							/*!< brief Gyro factor / weight for the calculations */
	
	float axr;
	float ayr;
	float azr;
	
	float pitch;
	float roll;
	float yaw;
}FILTER_DATA_t;

void filters_init(FILTER_DATA_t *filterData);
void filters_calculate_five_degrees_of_freedom(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr);
void filters_calculate_six_degrees_of_freedom(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr);
#endif /* FILTERS_H_ */