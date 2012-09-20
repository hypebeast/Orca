/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA ADC driver example source.
 *
 *      This file contains an example application that demonstrates the ADC
 *      driver. It shows how to set up the ADC to measure the voltage on four
 *      analog inputs. It uses the internal VCC/1.6 voltage as conversion reference.
 *      A number of samples are stored in an SRAM table for later examination.
 *
 *      The recommended test setup for this application is to connect five
 *      10k resistors in series between GND and VCC source and connect the four
 *      resistor junctions to analog input ADCA4, ADCA5, ADCA6 and ADCA7.
 *
 *      To achieve the best performance, the voltage reference can be decoupled
 *      through the external ADC reference pins.
 *
 * \par Application note:
 *      AVR1300: Using the XMEGA ADC
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 644 $
 * $Date: 2007-10-02 11:17:38 +0200 (ty, 02 okt 2007) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include "avr_compiler.h"
#include "adc_driver.h"

/*! How many samples for each ADC channel.*/
#define SAMPLE_COUNT 10

/*! Sample storage (all four channels).*/
int16_t adcSamples[4][SAMPLE_COUNT];

volatile int8_t offset;

int main(void)
{
	/* Move stored calibration values to ADC A. */
	ADC_CalibrationValues_Load(&ADCA);

	/* Set up ADC A to have signed conversion mode and 12 bit resolution. */
  	ADC_ConvMode_and_Resolution_Config(&ADCA, ADC_ConvMode_Signed, ADC_RESOLUTION_12BIT_gc);

	/* Set sample rate. */
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV32_gc);

	/* Set reference voltage on ADC A to be VCC/1.6 V.*/
	ADC_Reference_Config(&ADCA, ADC_REFSEL_VCC_gc); 

	/* Setup channel 0, 1, 2 and 3 with different inputs. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_DIFF_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH1,
	                                 ADC_CH_INPUTMODE_INTERNAL_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH2,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
                                     ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH3,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

   	/* Get offset value for ADC A. */
   	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN1_gc);

	ADC_Enable(&ADCA);
	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);
 	offset = ADC_Offset_Get_Signed(&ADCA, &ADCA.CH0, false);
    ADC_Disable(&ADCA);
    
	/* Set input to the channels in ADC A */
	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN2_gc, ADC_CH_MUXNEG_PIN3_gc);
	ADC_Ch_InputMux_Config(&ADCA.CH1, ADC_CH_MUXINT_SCALEDVCC_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH2, ADC_CH_MUXPOS_PIN3_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH3, ADC_CH_MUXPOS_PIN4_gc, 0);

	/* Setup sweep of all four virtual channels. */
	ADC_SweepChannels_Config(&ADCA, ADC_SWEEP_0123_gc);

	/* Enable ADC A .*/
	ADC_Enable(&ADCA);

	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);

	/* Enable free running mode. */
	ADC_FreeRunning_Enable(&ADCA);

	/* Store samples in table.*/
	for (uint16_t i = 0; i < SAMPLE_COUNT; ++i) {

	  	do{
			/* If the conversion on the ADCA channel 0 never is
			 * complete this will be a deadlock. */
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH0));
		adcSamples[0][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH0, offset);

		do{
			/* If the conversion on the ADCA channel 1 never is
			 * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH1));
		adcSamples[1][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH1, offset);

		do{
			/* If the conversion on the ADCA channel 2 never is
			 * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH2));
		adcSamples[2][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH2, offset);

		do{
		  /* If the conversion on the ADCA channel 3 never is
		   * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH3));
		adcSamples[3][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH3, offset);
	}

	/* Turn off free running and disable ADC module.*/
	ADC_FreeRunning_Disable(&ADCA);
	ADC_Disable(&ADCA);

	/* Infinite loop.*/
	while (true){
	}
}


