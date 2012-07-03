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
#include "serial_api.h"
#include <rtc.h>

//just for test
#include "system_info.h"
#include "user_board.h"
#include "orca_init.h"
#include "servo_in.h"
#include "servo.h"
#include "flight_controller.h"
#include "voltage_sens.h"

/* global variables */
BOARD_CONFIG_t board;  						/*!< \brief board module */
SERVO_IN_t servoIn;							/*!< \brief servo input module */
FLIGHT_CONTROLLER_t flightController;		/*!< \brief flight controller module */
VOLTAGE_SENSOR_t voltageSensor;				/*!< \brief voltage Sensor module */

 static void ISR_system_timer(void)
 {
	nop();
 }
 
int main (void)
{
	// Initialize all basic board functions
	orca_init();
	
	//testing stuff	
	Stat_LED_ON();
	Err_LED_ON();
	
	//voltage_sens_write_Setup(0);
	
	while(1)
	{
		// Process incoming API commands
		serial_api_task();
		voltage_sens_task(&voltageSensor);
		// Flight controller
		//flight_controller_task(&flightController);		
	}
}

void orca_init()
{
	//struct pll_config pllcfg;
				
	osc_enable(OSC_ID_RC32MHZ);
    osc_wait_ready(OSC_ID_RC32MHZ); 
	//pll_enable_config_defaults(0); 
	
		pmic_init();
		
	/* init system timer */
	sysclk_init();
	
	/* Init delay functions */
	delay_init(sysclk_get_cpu_hz());
	
	/* board init */
	orca_board_init(&board);
	
	/* servo in subsystem init */
	servo_in_init(&board, &servoIn);
	
	/* servo out subsystem init */
	servo_init();
	
	/* flight controller subsystem init */
	flight_controller_init(&board, &servoIn, &flightController);

	/* Initialize the serial interface */
	serial_api_init();
	
	/* Initialize the I2C intern interface*/
	i2c_intern_init();
	
	cpu_irq_enable();
		
	rtc_init();
	rtc_set_callback(system_timer);
		

	
	delay_ms(300);
	
	/* Initialize the the voltage sensor */
	voltage_sens_init(&voltageSensor, 0x02);
	
	//tc_enable(&TCC0);
	//tc_set_overflow_interrupt_callback(&TCC0, ISR_system_timer);
	//tc_set_wgm(&TCC0, TC_WG_NORMAL);
	//tc_write_period(&TCC0, 1000);
	//tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_HI);

	/* enable interrupts */
	/* Enables all interrupt levels, with vectors located in the application section and fixed priority scheduling */
	//pmic_init();
	//cpu_irq_enable();
	
	//tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
	

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
}

static void system_timer(uint32_t time)
{
	rtc_set_alarm(time);
}
 
ISR(PORTH_INT0_vect)
{
	isr_servo_in(&servoIn);	
}
