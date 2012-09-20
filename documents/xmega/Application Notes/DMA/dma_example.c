/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DMA driver example source.
 *
 *      This file contains an example application that demonstrates the DMA
 *      driver. It shows how to set up the DMA for a memory-to-memory block
 *      copy operation. Two functions are provided, one using single block
 *      transfer and one using repeated block transfer. Single block transfers
 *      supports up to 64k bytes, while repeated blocks supports up to 255
 *      blocks (16M - 64k bytes). Both functions are tested in the main()
 *      function.
 *
 * \par Application note:
 *      AVR1304: Using the XMEGA DMA Controller
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
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
#include "dma_driver.h"
#include "avr_compiler.h"

/*! Size of one memory block. */
#define MEM_BLOCK_SIZE   (10)

 /*! Memory block count. */
#define MEM_BLOCK_COUNT (10)

/*! Total block size. Note that MEM_BLOCK_SIZE * MEM_BLOCK_COUNT must not
 *  exceed 64k for this demo application.
 */
#define MEM_BLOCK ( MEM_BLOCK_SIZE * MEM_BLOCK_COUNT )

/*! Memory blocks A and B for testing. */
uint8_t memoryBlockA[MEM_BLOCK];
uint8_t memoryBlockB[MEM_BLOCK];

/*! Global declared status for interrupt routine. */
volatile bool gInterruptDone;
volatile bool gStatus;

/*! \brief Example demonstrating block memory copy.
 *
 *  Block size 0 = 64k. Enable the DMA channel to use first and it will be
 *  disabled automatically. Setup channel for copying data, increasing
 *  addresses, no address pointer reload, with 8-byte bursts.
 *
 * \note This function DO NOT wait for a completion, and this must be handled
 *       by the program calling this function. In this example, an interrupt.
 *
 * \retval true   if success.
 * \retval false  if failure.
 */
bool BlockMemCopy( const void * src,
                   void * dest,
                   uint16_t blockSize,
                   volatile DMA_CH_t * dmaChannel )
{
	DMA_EnableChannel( dmaChannel );

	DMA_SetupBlock( dmaChannel,
	                src,
	                DMA_CH_SRCRELOAD_NONE_gc,
	                DMA_CH_SRCDIR_INC_gc,
	                dest,
	                DMA_CH_DESTRELOAD_NONE_gc,
	                DMA_CH_DESTDIR_INC_gc,
	                blockSize,
	                DMA_CH_BURSTLEN_8BYTE_gc,
	                0,
	                false );

	DMA_StartTransfer( dmaChannel );

	return true;
}



/*! \brief Example of a repeated block memory copy operation.
 *
 *  Block size 0 = 64k. Enable the DMA channel to use first and the channel
 *  will be disabled automatically. A parameter check to avoid illegal values.
 *  Setup channel for copying data, increasing addresses, no address pointer
 *  reload, with 8-byte bursts.
 *
 * \note This function wait until the transfer is complete and use a blocking
 *       function which also makes this function blocking, hence the function
 *       will dead-lock if the completion or error flag never get set.
 *
 * \retval true   if success.
 * \retval false  if failure.
 */
bool MultiBlockMemCopy( const void * src, void * dest, uint16_t blockSize,
                          uint8_t repeatCount, volatile DMA_CH_t * dmaChannel )
{
	uint8_t flags;

	DMA_EnableChannel( dmaChannel );
	DMA_SetupBlock( dmaChannel,
	                src,
	                DMA_CH_SRCRELOAD_NONE_gc,
	                DMA_CH_SRCDIR_INC_gc,
	                dest,
	                DMA_CH_DESTRELOAD_NONE_gc,
	                DMA_CH_DESTDIR_INC_gc,
	                blockSize,
	                DMA_CH_BURSTLEN_8BYTE_gc,
	                repeatCount,
	                true );

	DMA_StartTransfer( dmaChannel );

	/* Wait until the completion or error flag is set. The flags
	 * must be cleared manually.
	 */
	do {
		flags = DMA_ReturnStatus_non_blocking( dmaChannel );
	} while ( flags == 0);

	dmaChannel->CTRLB |= ( flags );

	/* Check if error flag is set. */
	if ( ( flags & DMA_CH_ERRIF_bm ) != 0x00 ) {
		return false;
	} else {
		return true;
	}
}

/*! \brief Example code using two different methods of transfer.
 *
 *  Example code using the two example functions to do a singe block memory
 *  copy and a multi block memory copy. The first test use a polling function
 *  to wait until the transfer completes or get an error. The second test use
 *  an interrupt to know when the transfer completes or get an error.
 */
void main( void )
{
	uint32_t index;

	volatile DMA_CH_t * Channel;
	Channel = &DMA.CH0;

	DMA_Enable();

	/* Test 1: Copy using repeated blocks. */

	/* Fill memory block A with example data. */
	for ( index = 0; index < MEM_BLOCK; ++index ) {
		memoryBlockA[index] = ( (uint8_t) index & 0xff );
	}

	/* Copy data using channel 0. */
	gStatus = MultiBlockMemCopy( memoryBlockA,
	                             memoryBlockB,
	                             MEM_BLOCK_SIZE,
	                             MEM_BLOCK_COUNT,
	                             Channel );

	/* Compare memory blocks if status is true. */
	if ( gStatus ) {
		for ( index = 0; index < MEM_BLOCK; ++index) {
			if (memoryBlockA[index] != memoryBlockB[index]) {
				gStatus = false;
				break;
			}
		}
	}


	/*  Test 2: Copy using single block mode and use interrupt. Perform
	 *  second test only if first test succeeded.
	 */
	if ( gStatus ) {

		/*  Enable LO interrupt level for the complete transaction and
		 *  error flag on DMA channel 0.
		 */
		DMA_SetIntLevel( Channel, DMA_CH_TRNINTLVL_LO_gc, DMA_CH_ERRINTLVL_LO_gc );
		PMIC.CTRL |= PMIC_LOLVLEN_bm;
		sei();

		/* Fill memory block A with example data again. */
		for ( index = 0; index < MEM_BLOCK; ++index ) {
			memoryBlockA[index] = 0xff - ( (uint8_t) index & 0xff );
		}

		/* Set intDone to false to know when it has been executed. */
		gInterruptDone = false;

		/* Copy data using channel 0. */
		gStatus = BlockMemCopy( memoryBlockA,
		                        memoryBlockB,
		                        MEM_BLOCK,
		                        Channel);

		do {
			/* Do something here while waiting for the
			 * interrupt routine to signal completion.
			 */
		} while ( gInterruptDone == false );

		/* Compare memory blocks. */
		if ( gStatus ) {
			for ( index = 0; index < MEM_BLOCK; ++index ) {
				if (memoryBlockA[index] != memoryBlockB[index]) {
					gStatus = false;
					break;
				}
			}
		}
	}

	if ( gStatus ) {
		do {
			/* Completed with success. */
		} while (1);
	} else {
		do {
			/* Completed with failure. */
		} while (1);
	}
}


/*! DMA CH0 Interrupt service routine. Clear interrupt flags after check. */
ISR(DMA_CH0_vect)
{
	if (DMA.CH0.CTRLB & DMA_CH_ERRIF_bm) {
		DMA.CH0.CTRLB |= DMA_CH_ERRIF_bm;
		gStatus = false;
	} else {
		DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm;
		gStatus = true;
	}
	gInterruptDone = true;
}
