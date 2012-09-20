/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA ADC driver interrupt example source.
 *
 *      This file contains an example application that demonstrates the ADC
 *       driver using interrupts. It shows how to set up the ADC to measure
 *      the voltage on four analog inputs. It uses the Internal reference of
 *      VCC/1.6 V as conversion reference. A number of samples are stored in
 *      an SRAM table for later examination.
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
 * $Revision: 596 $
 * $Date: 2007-03-22 14:43:48 +0100 (to, 22 mar 2007) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
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
#define SAMPLE_COUNT 100

/*! Sample storage (all four channels).*/
int16_t adcSamples[4][SAMPLE_COUNT];

uint16_t interrupt_count = 0;
int8_t offset;

int main(void)
{
	/* Move stored calibration values to ADC A. */
	ADC_CalibrationValues_Load(&ADCA);

	/* Set up ADC A to have signed conversion mode and 12 bit resolution. */
  	ADC_ConvMode_and_Resolution_Config(&ADCA, ADC_ConvMode_Signed, ADC_RESOLUTION_12BIT_gc);

	/* Set sample rate */
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV32_gc);

	/* Set referance voltage on ADC A to be VCC/1.6 V.*/
	ADC_Reference_Config(&ADCA, ADC_REFSEL_VCC_gc);

   	/* Get offset value for ADC A. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_DIFF_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

   	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN1_gc);

	ADC_Enable(&ADCA);
	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);
 	offset = ADC_Offset_Get_Signed(&ADCA, &ADCA.CH0, false);
    ADC_Disable(&ADCA);
    
	/* Setup channel 0, 1, 2 and 3 to have single ended input. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH1,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH2,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH3,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);
    
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);
    
	/* Set input to the channels in ADC A to be PIN 4, 5, 6 and 7. */
	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN4_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH1, ADC_CH_MUXPOS_PIN5_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH2, ADC_CH_MUXPOS_PIN6_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH3, ADC_CH_MUXPOS_PIN7_gc, 0);

	/* Setup sweep of all four virtual channels.*/
	ADC_SweepChannels_Config(&ADCA, ADC_SWEEP_0123_gc);

	/* Enable low level interrupts on ADCA channel 3, on conversion complete. */
	ADC_Ch_Interrupts_Config(&ADCA.CH3, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);

	/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts. */
	sei();

	/* Enable ADC A with free running mode, VCC reference and signed conversion.*/
	ADC_Enable(&ADCA);

	/* Wait until common mode voltage is stable. Default clock is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);

	/* Enable free running mode. */
	ADC_FreeRunning_Enable(&ADCA);

	/* Eternal loop to wait for the conversions and interrupts to finnish. */
	while(true){
	}
}


/*! Interrupt routine that reads out the result form the conversion on all
 *  channels to a global array. If the number of conversions carried out is less
 *  than the number given in the define SAMPLE_COUNT a new conversion on all the
 *  channels is started. If not the interrupt on ADC A channel 3 is disabled.
 *
 */

ISR(ADCA_CH3_vect)
{
	/*  Read samples and clear interrupt flags. */
  	adcSamples[0][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH0, offset);
	adcSamples[1][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH1, offset);
	adcSamples[2][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH2, offset);
	adcSamples[3][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH3, offset);


  	if(interrupt_count == SAMPLE_COUNT-1){
		ADC_Ch_Interrupts_Config(&ADCA.CH3, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_OFF_gc);
		ADC_FreeRunning_Disable(&ADCA);
		ADC_Disable(&ADCA);
	}
	interrupt_count++;
}
