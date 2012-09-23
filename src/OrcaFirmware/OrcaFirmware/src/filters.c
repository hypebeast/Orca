/*
 * filters.c
 *
 * Created: 23.09.2012 15:23:46
 *  Author: f-ruml
 */ 

#include "sysclk.h"

/*To add the "math.h" go to the Project properties under "Toolchain" look to the "AVR/GNU C Linker" the second subcategory there is "Libraries".
In the top of the two boxes make an entry for "libm.a" */
#include "math.h" 
#include "filters.h"


float filters_squared(float x);
void filters_normalize_3DVector(float* vector);

FILTER_DATA_t *filter;

//Notation "w" stands for one of the axes, so for example RwAcc[0],RwAcc[1],RwAcc[2] means RxAcc,RyAcc,RzAcc
//Variables below must be global (their previous value is used in getEstimatedInclination)
float RwEst[3];     //Rw estimated from combining RwAcc and RwGyro
unsigned long lastMicros;

//Variables below don't need to be global but we expose them for debug purposes
unsigned long interval; //interval since previous analog samples
float RwAcc[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float RwGyro[3];        //Rw obtained from last estimated value and gyro movement
float Awz[2];           //angles between projection of R on XZ/YZ plane and Z axis (deg)


void filters_init(FILTER_DATA_t *filterData)
{
	filter = filterData;
	
	filter->firstSample = 1;
	filter->gyroFactor = FILTERS_GYRO_FACTOR_CONF;	
}


void filters_get_estimated_inclination(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr)
{
	static int w;
	static float tmpf;  
	static char signRzGyro;  
	
	RwAcc[0] = xAcc;
	RwAcc[1] = yAcc;
	RwAcc[2] = zAcc;
	
	float wgyr[3]={xGyr, yGyr, zGyr};
		
	/* compute interval since last sampling time */
	interval = time - lastMicros;    //please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 
	lastMicros = time;               //save for next loop, please note interval will be invalid in first sample but we don't use it
	
	/* normalize vector (convert to a vector with same direction and with length 1) */
	filters_normalize_3DVector(RwAcc);
  
	if(filter->firstSample == 1)
	{
		for(w=0;w<=2;w++)
		{
			RwEst[w] = RwAcc[w];    //initialize with accelerometer readings
		}		 
	}
	else
	{
		//evaluate RwGyro vector
		if(fabs(RwEst[2]) < 0.1)
		{
			//Rz is too small and because it is used as reference for computing Axz, Ayz it's error fluctuations will amplify leading to bad results
			//in this case skip the gyro data and just use previous estimate
			for(w=0;w<=2;w++)
			{
				RwGyro[w] = RwEst[w];
			}
		}			
		else
		{
			//get angles between projection of R on ZX/ZY plane and Z axis, based on last RwEst
			for(w=0;w<=1;w++)
			{
				tmpf = wgyr[w];									//get current gyro rate in deg/ms
				tmpf *= interval / 1000.0f;                     //get angle change in deg
				Awz[w] = atan2(RwEst[w],RwEst[2]) * 180 / M_PI;							//get angle and convert to degrees        
				Awz[w] += tmpf;                                 //get updated angle according to gyro movement
			 }
      
			//estimate sign of RzGyro by looking in what qudrant the angle Axz is, 
			//RzGyro is pozitive if  Axz in range -90 ..90 => cos(Awz) >= 0
			
			signRzGyro = ( cos(Awz[0] * PI / 180) >=0 ) ? 1 : -1;
      
			//reverse calculation of RwGyro from Awz angles, for formula deductions see  http://starlino.com/imu_guide.html
			for(w=0;w<=1;w++)
			{
				RwGyro[w] = sin(Awz[w] * PI / 180);
				RwGyro[w] /= sqrt( 1 + filters_squared(cos(Awz[w] * PI / 180)) * filters_squared(tan(Awz[1-w] * PI / 180)) );
			}
			RwGyro[2] = signRzGyro * sqrt(1 - filters_squared(RwGyro[0]) - filters_squared(RwGyro[1]));
		}
    
		//combine Accelerometer and gyro readings
		for(w=0;w<=2;w++) RwEst[w] = (RwAcc[w] + (filter->gyroFactor)* RwGyro[w]) / (1 + (filter->gyroFactor));

		filters_normalize_3DVector(RwEst);
		filter->axr = RwEst[0];
		filter->ayr = RwEst[1];
		filter->azr = RwEst[2];   
	}
  
  filter->firstSample = 0;
}

void filters_normalize_3DVector(float* vector)
{
  static float R;  
  R = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
  vector[0] /= R;
  vector[1] /= R;  
  vector[2] /= R;  
}

float filters_squared(float x)
{
  return x*x;
}
