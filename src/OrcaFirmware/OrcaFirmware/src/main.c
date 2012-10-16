/**
 * \file main.c
 *
 * \brief Entry point for the Orca firmware program.
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include <pmic.h>
#include <pll.h>
#include <osc.h>
#include <delay.h>
#include <rtc.h>

#include "system_info.h"
#include "user_board.h"
#include "serial_api.h"
#include "orca_init.h"
#include "servo_in.h"
#include "servo.h"
#include "flight_controller.h"
#include "voltage_sens.h"
#include "MPU6000.h"
#include "user_interface.h"
#include "serial_flash.h"
#include "filters.h"

/* global variables */
BOARD_CONFIG_t board;  								/*!< \brief board module */
SERVO_IN_t servoInput;									/*!< \brief servo input module */
FLIGHT_CONTROLLER_t flightController;				/*!< \brief flight controller module */
VOLTAGE_SENSOR_t voltageSensor;						/*!< \brief voltage Sensor module */
MOTION_PROCESSING_UNIT_t motionProcessingUnit;		/*!< \brief motion processing unit module */
FILTER_DATA_t orcafilter;							/*!< \brief filter module */
ORCA_FLASH_SETTINGS_t orcaSettings;					/*!< \brief orca settings module */

unsigned long ulFcTickCounter = 0;			/*!< \brief Flight Controller system tick counter */
unsigned long ulVsTickCounter = 0;			/*!< \brief Voltage sensor system tick counter */
unsigned long ulUiTickCounter = 0;			/*!< \brief User Interface system tick counter */

/*! \brief Entry point for the program.
 *
 * This function initialize everything and starts the main loop.
 */
int main (void)
{
	/* Initialize all basic board functions */
	orca_init();
	
	user_interface_stat_led_pattern(USER_INTERFACE_LED_BLINKING);
	/* Uncomment this method to restore the factory settings on the next startup */
	//serial_flash_factory_reset();
	
	/* Calibrate the accelerometer and the gyroscopes */
	mpu_6000_calibrate();
	
	/* Set the LEDs to their initial state */
	user_interface_stat_led_pattern(USER_INTERFACE_LED_SINGLE_FLASH);
			
	// Loop forever
	while(1)
	{
		// Process incoming API commands from the USB USART
		serial_api_task();
		
		// Flight controller
		flight_controller_task(&flightController);
		

	}
	
	return 0;
}


/*! \brief Every subsystem gets initialized by this function
 *
 * All subsystem is initialized in this function. It's called by the main function.
 */
void orca_init(void)
{
	//struct pll_config pcfg;
	
	/* Disable all interrupts */
	cpu_irq_disable();
	
	/* Initialize clock */	
	sysclk_init();	
	
	osc_enable(OSC_ID_RC32MHZ);
	osc_enable(OSC_ID_RC32KHZ);
    osc_wait_ready(OSC_ID_RC32MHZ); 	
	
	//pll_config_init(&pcfg, PLL_SRC_RC32MHZ, 4, 4);
	//pll_enable(&pcfg, 0);
	//do {} while (!pll_is_locked(0));
	//sysclk_set_prescalers(SYSCLK_PSADIV_1, SYSCLK_PSBCDIV_1_1);
	//sysclk_set_source(SYSCLK_SRC_PLL);
	
	/* Initialize delay functions */
	delay_init(sysclk_get_cpu_hz());
	
	/* Board Init */
	orca_board_init(&board);
	
	/* Initialize serial flash and get settings */
	if(serial_flash_init() == true)
	{
		serial_flash_init_factory_settings(&orcaSettings);
	}
		
	/* servo in subsystem init */
	servo_in_init(&board, &servoInput);
	
	/* Initialize the servo output subsystem */
	servo_init();
	
	/* flight controller subsystem init */
	flight_controller_init(&board, &servoInput, &orcafilter, &flightController);

	/* Initialize the serial interface */
	serial_api_init();
	
	/* Initialize the I2C intern interface*/
	i2c_intern_init();
		
	/* Wait some time for external devices/sensors to start up */	
	delay_ms(300);
	
	/* Initialize the the voltage sensor */
	voltage_sens_init(&voltageSensor, 0x02);
	
	/* Initialize the gyro/acc sensor*/
	mpu_6000_init(&motionProcessingUnit);
	
	/* Initialize the filter module */
	filter_init(&orcafilter, orcaSettings.Q_angle, orcaSettings.Q_gyro, orcaSettings.R_angle);
			
	/* Enables all interrupt levels, with vectors located in the application section and fixed priority scheduling */
	pmic_init();

	/* Enable interrupts */
	cpu_irq_enable();

	delay_ms(100);
	
	/* Initialize and start the System Timer */
	rtc_init();
	rtc_set_callback(system_timer);
	rtc_set_alarm(1);
	rtc_set_alarm_relative(0);
	rtc_set_time(0);
}

uint16_t i2c_intern_init(void)
{
	twi_options_t opt;
	opt.speed = VOLTAGE_SENS_I2C_SPEED;
	opt.chip = VOLTAGE_SENS_DEV_ADDRESS;
	opt.speed_reg = TWI_BAUD(32000000,VOLTAGE_SENS_I2C_SPEED);
	
	sysclk_enable_peripheral_clock(BOARD_I2C_INTERN_INTERFACE);
		
	/* Init TWI interface */
	if(twi_master_init(BOARD_I2C_INTERN_INTERFACE, &opt) != STATUS_OK)
	return SYSTEM_INFO_VOLTAGE_SENSOR_INIT_ERR;

	twi_master_enable(BOARD_I2C_INTERN_INTERFACE);
	
	return SYSTEM_INFO_TRUE;
}

/**************************************************************************
* \brief System Timer Callback
*	This function is called periodically every 10 ms. Use this function 
*	for scheduling tasks. /n
*
* \param *time time
*
* \return  ---
***************************************************************************/
void system_timer(uint32_t time)
{
	ulFcTickCounter++;
	ulVsTickCounter++;
	ulUiTickCounter++;
	
	/* Call the flight Controller every 10 ms */
	if(ulFcTickCounter >= 1)
	{
		/* Wait until MPU 6000 is calibrated */
		if(motionProcessingUnit.state == MPU_6000_STATE_RUN)
		{
			/* Get new data from the mpu6000 */
			mpu_6000_task();
			/* Save the new measurements to the filter module */
			mpu_6000_save_data_to_filter(&orcafilter);
			/* Do the kalman filter */
			filter_task(motionProcessingUnit.time);
		}
		ulFcTickCounter = 0;
	}
			
	/* Call the voltage sensor every 500 */
	if(ulVsTickCounter >= 50)
	{
		voltage_sens_task(&voltageSensor);
		ulVsTickCounter = 0;
	}
			
	/* Update the user interface every 50 ms */
	if(ulUiTickCounter >= 5)
	{
		user_interface_update_LEDs();
		ulUiTickCounter = 0;
	}
	
	rtc_set_alarm(1);
	rtc_set_time(0);
}


ISR(PORTA_INT0_vect)
{
	//isr_servo_in(&servoIn);
}