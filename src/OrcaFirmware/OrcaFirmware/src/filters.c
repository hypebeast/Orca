*
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
void dcm_orthonormalize(float dcm[3][3]);
void dcm_rotate(float dcm[3][3], float w[3]);
float vector3d_modulus(float* vector);
void vector3d_normalize(float* vector);
float vector3d_dot(float* a,float* b);
void vector3d_cross(float* a,float* b, float* c);
void vector3d_scale(float s, float* a , float* b);
void vector3d_add(float* a , float* b, float* c);
void vector3d_skew_plus_identity(float *v,float* m);
void filters_euler_angles(float dcm[3][3]);

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

/*************************************** 6 DOF ***********************************************************/
unsigned int imu_sequence = 0;			//incremented on each call to imu_update
unsigned int count250us_prev;

float dcmAcc[3][3];                     //dcm matrix according to accelerometer
float dcmGyro[3][3];                    //dcm matrix according to gyroscopes
float dcmEst[3][3];                     //estimated dcm matrix by fusion of accelerometer and gyro

void filters_init(FILTER_DATA_t *filterData)
{
	filter = filterData;
	
	filter->firstSample = 1;
	filter->gyroFactor = FILTERS_GYRO_FACTOR_CONF;	
	
	/* For 6 dof */
	unsigned char i,j;
        for(i=0;i<3;i++)
                for(j=0;j<3;j++)
                        dcmGyro[i][j] = (i==j) ? 1.0 : 0.0;
	
}


void filters_calculate_five_degrees_of_freedom(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr)
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
				RwGyro[w] /= sqrt( 1 + filters_squared(cos(Awz[w] * PI / 180)) * filters_squared(tan(Awz[1-w] * PI / 180)));
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


/* Based on http://www.starlino.com/dcm_tutorial.html */
void filters_calculate_six_degrees_of_freedom(unsigned long time, float xAcc, float yAcc, float zAcc, float xGyr, float yGyr, float zGyr)
{
	int i;
	imu_sequence++;

	/* compute interval since last sampling time */
	float interval_ms = (time - lastMicros) /1000;    //please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 
	lastMicros = time;               //save for next loop, please note interval will be invalid in first sample but we don't use it
	
	 //interval since last call
	//float interval_ms = 0.25*(float)(count250us-count250us_prev);
	//count250us_prev = count250us;

	//---------------
	// I,J,K unity vectors of global coordinate system I-North,J-West,K-zenith
	// i,j,k unity vectors of body's coordiante system  i-"nose", j-"left wing", k-"top"
	//---------------
	//                      [I.i , I.j, I.k]
	// DCM =        [J.i , J.j, J.k]
	//                      [K.i , K.j, K.k]  


	//---------------
	//Acelerometer
	//---------------
	//Accelerometer measures gravity vector G in body coordinate system
	//Gravity vector is the reverse of K unity vector of global system expressed in local coordinates
	//K vector coincides with the z coordinate of body's i,j,k vectors expressed in global coordinates (K.i , K.j, K.k)               
	//Acc can estimate global K vector(zenith) measured in body's coordinate systems (the reverse of gravitation vector)
	float Kacc[3] = {-xAcc, -yAcc, -zAcc}; 
		
	vector3d_normalize(Kacc);
	
	//calculate correction vector to bring dcmGyro's K vector closer to Acc vector (K vector according to accelerometer)
	float wA[3]; 
	vector3d_cross(dcmGyro[2],Kacc,wA);     // wA = Kgyro x  Kacc , rotation needed to bring Kacc to Kgyro

	//---------------
	//Magnetomer
	//---------------
	//calculate correction vector to bring dcmGyro's I vector closer to Mag vector (I vector according to magnetometer)
	float Imag[3];  
	float wM[3]; 
	//in the absense of magnetometer let's assume North vector (I) is always in XZ plane of the device (y coordinate is 0)
	Imag[0] = sqrt(1-dcmGyro[0][2]*dcmGyro[0][2]);
	Imag[1] = 0;
	Imag[2] = dcmGyro[0][2];
        
	vector3d_cross(dcmGyro[0],Imag,wM);     // wM = Igyro x Imag, roation needed to bring Imag to Igyro

	//---------------
	//dcmGyro
	//---------------
	float w[3];         //gyro rates (angular velocity of a global vector in local coordinates)
	w[0] = -xGyr;       //rotation rate about accelerometer's X axis (GY output) in rad/ms
	w[1] = -yGyr;       //rotation rate about accelerometer's Y axis (GX output) in rad/ms
	w[2] = -zGyr;       //rotation rate about accelerometer's Z axis (GZ output) in rad/ms
	for(i=0;i<3;i++)
	{
		w[i] *= interval_ms;  //scale by elapsed time to get angle in radians
		//compute weighted average with the accelerometer correction vector
		w[i] = (w[i] + ACC_WEIGHT*wA[i] + MAG_WEIGHT*wM[i])/(1.0+ACC_WEIGHT+MAG_WEIGHT);
	}
        
	dcm_rotate(dcmGyro,w);

	filters_euler_angles(dcmGyro);

        //Output for PicQuadController_GYRO_DEBUG1.scc
        //only output data ocasionally to allow computer to process data
        /*
        if(0 == imu_sequence % 4){
                printf("%.5f,",(double)interval_ms);
                print_float_list(3,(float*)w);
                printf(",%.2f,%.2f,%.2f",adcAvg[3+1],adcAvg[3+0],adcAvg[3+2]);

                printf("\n ");
        }
        */

        
        //Output for: PICQUADCONTROLLER_DEBUG1.pde
        //only output data ocasionally to allow computer to process data
        //if(0 == imu_sequence % 16){
                //printf("%.2f,",(double)interval_ms);
                //print_float_list(3,(float*)Kacc);
                //printf(", ");
                //print_float_list(9,(float*)dcmGyro);
                //printf("\n");
        //}
}

void filters_euler_angles(float dcm[3][3])
{
	filter->yaw = atan2(dcm[2][0],dcm[2][1]);
	filter->pitch = acos(dcm[2][2]);
	filter->roll = -atan2(dcm[0][2],dcm[1][2]);
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

//bring dcm matrix in order - adjust values to make orthonormal (or at least closer to orthonormal)
//Note: dcm and dcmResult can be the same
void dcm_orthonormalize(float dcm[3][3]){
        //err = X . Y ,  X = X - err/2 * Y , Y = Y - err/2 * X  (DCMDraft2 Eqn.19)
        float err = vector3d_dot((float*)(dcm[0]),(float*)(dcm[1]));
        float delta[2][3];
        vector3d_scale(-err/2,(float*)(dcm[1]),(float*)(delta[0]));
        vector3d_scale(-err/2,(float*)(dcm[0]),(float*)(delta[1]));
        vector3d_add((float*)(dcm[0]),(float*)(delta[0]),(float*)(dcm[0]));
        vector3d_add((float*)(dcm[1]),(float*)(delta[0]),(float*)(dcm[1]));

        //Z = X x Y  (DCMDraft2 Eqn. 20) , 
        vector3d_cross((float*)(dcm[0]),(float*)(dcm[1]),(float*)(dcm[2]));
        //re-nomralization
        vector3d_normalize((float*)(dcm[0]));
        vector3d_normalize((float*)(dcm[1]));
        vector3d_normalize((float*)(dcm[2]));
}

//rotate DCM matrix by a small rotation given by angular rotation vector w
//see http://gentlenav.googlecode.com/files/DCMDraft2.pdf
void dcm_rotate(float dcm[3][3], float w[3]){
        //float W[3][3];        
        //creates equivalent skew symetric matrix plus identity matrix
        //vector3d_skew_plus_identity((float*)w,(float*)W);
        //float dcmTmp[3][3];
        //matrix_multiply(3,3,3,(float*)W,(float*)dcm,(float*)dcmTmp);
        
        int i;
        float dR[3];
        //update matrix using formula R(t+1)= R(t) + dR(t) = R(t) + w x R(t)
        for(i=0;i<3;i++){
                vector3d_cross(w,dcm[i],dR);
                vector3d_add(dcm[i],dR,dcm[i]);
        }               

        //make matrix orthonormal again
        dcm_orthonormalize(dcm);
}

//get modulus of a 3d vector sqrt(x^2+y^2+y^2)
float vector3d_modulus(float* vector){
        static float R;  
        R = vector[0]*vector[0];
        R += vector[1]*vector[1];
        R += vector[2]*vector[2];
        return sqrt(R);
}

//convert vector to a vector with same direction and modulus 1
void vector3d_normalize(float* vector){
        static float R;  
        R = vector3d_modulus(vector);
        vector[0] /= R;
        vector[1] /= R; 
        vector[2] /= R;  
}

//calcuate vector dot-product  c = a . b
float vector3d_dot(float* a,float* b){
        return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}


//calcuate vector cross-product  c = a x b
void vector3d_cross(float* a,float* b, float* c){
        c[0] = a[1]*b[2] - a[2]*b[1];
        c[1] = a[2]*b[0] - a[0]*b[2];
        c[2] = a[0]*b[1] - a[1]*b[0];
}

//calcuate vector scalar-product  n = s x a
void vector3d_scale(float s, float* a , float* b){
        b[0] = s*a[0];
        b[1] = s*a[1];
        b[2] = s*a[2];
} 


//calcuate vector sum   c = a + b
void vector3d_add(float* a , float* b, float* c){
        c[0] = a[0] + b[0];
        c[1] = a[1] + b[1];
        c[2] = a[2] + b[2];
} 


//creates equivalent skew symetric matrix plus identity
//for v = {x,y,z} returns
// m = {{1,-z,y}
//              {z,1,-x}
//              {-y,x,1}}
void vector3d_skew_plus_identity(float *v,float* m){
        m[0*3+0]=1;
        m[0*3+1]=-v[2];
        m[0*3+2]=v[1];
        m[1*3+0]=v[2];
        m[1*3+1]=1;
        m[1*3+2]=-v[0];
        m[2*3+0]=-v[1];
        m[2*3+1]=v[0];
        m[2*3+2]=1;
}
