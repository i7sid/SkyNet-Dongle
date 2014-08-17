/**
 * @file
 *
 * @date	24.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <string.h>



#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif


#include "spi.h"
#include "../misc/misc.h"
#include "../cpu/systick.h"

static SPI_CONFIG_FORMAT_T 	spi_format;


void SPI_Init(void)
{
	/* Set up clock and muxing for SPI (aka SSP0) interface */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI);

	/*
	 * Initialize SSP0 pins connect
	 * P0.15: SCK	F3 (SPI SCK)	PULLDOWN
	 * P0.16: SSEL	GPIO OUT		PULLUP
	 * P0.17: MISO	F3 (SPI MISO)	-
	 * P0.18: MOSI	F3 (SPI MOSI)	-
	 */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, SI_LIB_NSEL_PORT, SI_LIB_NSEL_PIN);
	SPI_DeassertSSEL();  // assure that SSEL is HIGH and radio chip can relax

	Chip_SPI_Init(LPC_SPI);
	spi_format.bits = SPI_BITS_8;				// one frame has 8 bits
	spi_format.clockMode = SPI_CLOCK_MODE0;		// standard clock mode
	spi_format.dataOrder = SPI_DATA_MSB_FIRST;	// MSB first
	Chip_SPI_SetFormat(LPC_SPI, &spi_format);


	// TODO: setup GPIO0 port pin and handler
	// TODO: set SPI speed?

	DBG("SPI initialized.\n");
}

void SPI_Deinit(void)
{
	SPI_DeassertSSEL();  // assure that SSEL is HIGH and radio chip can relax
	Chip_SPI_DeInit(LPC_SPI);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SPI);
}

uint8_t SPI_Trans(uint8_t data)
{
	LPC_SPI->DR = data;
	while(!(LPC_SPI->SR & SPI_SR_SPIF));
	return LPC_SPI->DR;
}

void SPI_TransBatchWrite(uint8_t *data, uint8_t length)
{
	for(int i=0; i<length; i++)
		SPI_Trans(data[i]);
}

void SPI_TransBatchRead(uint8_t *data, uint8_t length)
{
	for(int i=0; i<length; i++)
		data[i] = SPI_Trans(0xFF);
}

INLINE void SPI_AssertSSEL(void)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO, SI_LIB_NSEL_PORT, SI_LIB_NSEL_PIN);

	// assure that pin is really set
	// (and avoid some silly reordering of compiler... hopefully)
	while (Chip_GPIO_GetPinState(LPC_GPIO, SI_LIB_NSEL_PORT, SI_LIB_NSEL_PIN)) {
		__NOP();
	}
}

INLINE void SPI_DeassertSSEL(void)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, SI_LIB_NSEL_PORT, SI_LIB_NSEL_PIN);

	// assure that pin is really set
	// (and avoid some silly reordering of compiler... hopefully)
	while (!Chip_GPIO_GetPinState(LPC_GPIO, SI_LIB_NSEL_PORT, SI_LIB_NSEL_PIN)) {
		__NOP();
	}
}
