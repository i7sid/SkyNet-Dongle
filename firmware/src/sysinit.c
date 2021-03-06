/*
 * @brief Common SystemInit function for LPC17xx/40xx chips
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013-14
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "misc/misc.h"

 #if defined(NO_BOARD_LIB)
 #include "chip.h"
 #else
 #include "board.h"
 #endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SECTOR_5_START      0x00005000
#define SECTOR_8_START      0x00008000
#define SECTOR_10_START     0x0000a000
#define SECTOR_16_START     0x00010000

#define USER_FLASH_START (SECTOR_16_START)

#define NVIC_NUM_VECTORS          (16 + 33)     // CORE + MCU Peripherals
#define NVIC_RAM_VECTOR_ADDRESS   (0x10000000)  // Location of vectors in RAM
static volatile uint32_t* vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;



/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;
#endif


/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	// radio
	{SI_LIB_SDN_PORT,  SI_LIB_SDN_PIN,   		IOCON_MODE_INACT | IOCON_FUNC0},
	{SI_LIB_nIRQ_PORT,  SI_LIB_nIRQ_PIN,   		IOCON_MODE_INACT | IOCON_FUNC0},
	{SI_LIB_GPIO0_PORT,  SI_LIB_GPIO0_PIN,		IOCON_MODE_INACT | IOCON_FUNC0},
	{0,  15,   									IOCON_MODE_PULLDOWN | IOCON_FUNC3},	// SPI sCKL
	{SI_LIB_NSEL_PORT,  SI_LIB_NSEL_PIN,		IOCON_MODE_INACT | IOCON_FUNC0},
	{0,  17,   									IOCON_MODE_INACT | IOCON_FUNC3},		// SPI MISO
	{0,  18,   									IOCON_MODE_INACT | IOCON_FUNC3},		// SPI MOSI
	{RADIO_ON_PORT,  RADIO_ON_PIN,   			IOCON_MODE_INACT | IOCON_FUNC0},

	// ADC
	//{ADC_PWR_PORT,	ADC_PWR_PIN,				IOCON_MODE_INACT | IOCON_FUNC0},
	{ADC_IN_PORT,	ADC_IN_PIN,					IOCON_MODE_INACT | IOCON_FUNC1},
	{ADC_EXT_IN_PORT,	ADC_EXT_IN_PIN,			IOCON_MODE_INACT | IOCON_FUNC1},

	// LED
	{LED_PORT,  LED_PIN,						IOCON_MODE_INACT | IOCON_FUNC0},

	// USB
	{USB_CONNECT_PORT,  USB_CONNECT_PIN,		IOCON_MODE_INACT | IOCON_FUNC1},
	{USB_D_PLUS_PORT,  USB_D_PLUS_PIN,			IOCON_MODE_INACT | IOCON_FUNC1},
	{USB_D_MINUS_PORT,  USB_D_MINUS_PIN,		IOCON_MODE_INACT | IOCON_FUNC1},

	// GPS
//#ifdef IS_BASESTATION
	{GPS_UART_TX_PORT, GPS_UART_TX_PIN,			IOCON_MODE_INACT | IOCON_FUNC2},
	{GPS_UART_RX_PORT, GPS_UART_RX_PIN,			IOCON_MODE_INACT | IOCON_FUNC2},
//#else
//	{GPS_UART_TX_PORT, GPS_UART_TX_PIN,			IOCON_MODE_INACT | IOCON_FUNC0},
//	{GPS_UART_RX_PORT, GPS_UART_RX_PIN,			IOCON_MODE_INACT | IOCON_FUNC0},
//#endif


	// reset unused pins
	// (By default they are set as inputs with pull up. See: AN10915
	//  To reduce current consumption, we disable the pull up.)
	{0, 0,   									IOCON_MODE_INACT | IOCON_FUNC0},
	{0, 1,   									IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	4,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	5,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	6,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	7,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	10,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	11,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	21,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	22,										IOCON_MODE_INACT | IOCON_FUNC0},
	//{0,	23,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	24,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	25,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	26,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	27,										IOCON_MODE_INACT | IOCON_FUNC0},
	{0,	28,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	0,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	1,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	4,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	8,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	9,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	10,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	14,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	15,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	16,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	17,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1, 18,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	19,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1, 20,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	22,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	23,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	24,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	25,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	26,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	27,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	28,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	29,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	30,										IOCON_MODE_INACT | IOCON_FUNC0},
	{1,	31,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	2,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	3,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	4,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	5,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	6,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	7,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	11,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	12,										IOCON_MODE_INACT | IOCON_FUNC0},
	{2,	13,										IOCON_MODE_INACT | IOCON_FUNC0},
	{4,	28,										IOCON_MODE_INACT | IOCON_FUNC0},
	{4,	29,										IOCON_MODE_INACT | IOCON_FUNC0},


};


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));


	// now set unused pins to output, state LOW
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 4);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 5);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 6);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 7);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 1);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 4);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 8);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 9);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 14);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 15);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 16);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 19);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 22);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 23);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 24);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 25);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 26);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 27);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 28);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 29);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 30);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 31);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 2);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 5);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 6);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 7);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 4, 28);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 4, 29);

	Chip_GPIO_SetPinState(LPC_GPIO, 0, 4, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 5, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 6, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 7, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 0, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 1, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 4, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 8, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 9, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 14, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 15, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 16, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 19, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 22, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 23, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 24, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 25, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 26, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 27, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 28, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 29, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 30, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 31, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 2, 2, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 2, 5, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 2, 6, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 2, 7, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 4, 28, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 4, 29, false);
}


/* Setup system clocking */
void SystemSetupClocking(void)
{
	Chip_SetupXtalClocking();

	/* Change the USB Clock Divider setting for the operation with PLL0.
		Divide value = (480/48) = 10 */
	Chip_Clock_SetUSBClockDiv(9); /* pre-minus 1 */

	/* Wait for PLL0 to lock */
	while (!Chip_Clock_IsMainPLLLocked()) {}

	/* Connect PLL0 */
	Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE | SYSCTL_PLL_CONNECT);

	/* Wait for PLL0 to be connected */
	while (!Chip_Clock_IsMainPLLConnected()) {}

	/* Setup USB PLL1 for a 48MHz clock
		Input clock rate (FIN) is main oscillator = 12MHz
		PLL1 Output = USBCLK = 48MHz = FIN * MSEL, so MSEL = 4.
		FCCO = USBCLK = USBCLK * 2 * P. It must be between 156 MHz to 320 MHz.
		so P = 2 and FCCO = 48MHz * 2 * 2 = 192MHz */
	Chip_Clock_SetupPLL(SYSCTL_USB_PLL, 3, 1); /* Multiply by 4, Divide by 2 */

	/* Setup FLASH access to 5 clocks (120MHz clock) */
	Chip_SYSCTL_SetFLASHAccess(FLASHTIM_120MHZ_CPU);
}

/* Set up and initialize hardware prior to call to main */
void SystemInit(void)
{
	unsigned int *pSCB_VTOR = (unsigned int *) 0xE000ED08;

#if defined(__IAR_SYSTEMS_ICC__)
	extern void *__vector_table;

	*pSCB_VTOR = (unsigned int) &__vector_table;
#elif defined(__CODE_RED)
	extern void *g_pfnVectors;

	*pSCB_VTOR = (unsigned int) &g_pfnVectors;
#elif defined(__ARMCC_VERSION)
	extern void *__Vectors;

	*pSCB_VTOR = (unsigned int) &__Vectors;
#endif

#if defined(__FPU_PRESENT) && __FPU_PRESENT == 1
	fpuInit();
#endif


	/* Chip specific SystemInit */
	Chip_SystemInit();

	Board_SetupMuxing();

	SystemSetupClocking();

/*
	// We do not need BOD, so think green and disable this power consumer
	Chip_SYSCTL_DisableBODReset();
	Chip_SYSCTL_DisableBOD();
*/

	SCB->VTOR = (USER_FLASH_START & 0x1FFFFF80); 			// TODO: woher?

	// copy the vector table to SRAM to enable debugging
	uint32_t *old_vectors = (uint32_t*)SCB->VTOR;
	for (int i=0; i<NVIC_NUM_VECTORS; i++) {
		vectors[i] = old_vectors[i];
	}

	SCB->VTOR = (uint32_t)vectors;


}

