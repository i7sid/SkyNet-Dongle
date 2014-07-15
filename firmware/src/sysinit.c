/*
 * @brief Common LPCOpen SystemInit function
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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
 #if defined(NO_BOARD_LIB)
 #include "chip.h"
 #else
 #include "board.h"
 #endif

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	// Bluetooth
	{0,  0,   IOCON_MODE_INACT | IOCON_FUNC2},	// TXD3
	{0,  1,   IOCON_MODE_INACT | IOCON_FUNC2},	// RXD3
	{2,  13,  IOCON_MODE_INACT | IOCON_FUNC0},	// AT
	{0,  11,  IOCON_MODE_INACT | IOCON_FUNC0},	// Reset

	// radio
	{2,  8,   IOCON_MODE_INACT | IOCON_FUNC0},	// SDN
	{0,  19,   IOCON_MODE_INACT | IOCON_FUNC1},	// nIRQ
	{0,  20,   IOCON_MODE_INACT | IOCON_FUNC0},	// GPIO0
	{0,  15,   IOCON_MODE_PULLDOWN | IOCON_FUNC3},	// SPI sCKL
	{0,  16,   IOCON_MODE_PULLUP | IOCON_FUNC0},	// SPI nSEL
	{0,  17,   IOCON_MODE_INACT | IOCON_FUNC3},	// SPI MISO
	{0,  18,   IOCON_MODE_INACT | IOCON_FUNC3},	// SPI MOSI

	// ADC
	{1,  0,   IOCON_MODE_INACT | IOCON_FUNC0},	// ADC PWR
	{0,  23,   IOCON_MODE_INACT | IOCON_FUNC1},	// ADC IN

	// charger
	{0,  22,   IOCON_MODE_PULLUP | IOCON_FUNC0},	// STAT1 (open drain output!)
	{0,  21,   IOCON_MODE_PULLUP | IOCON_FUNC0},	// STAT2 (open drain output!)
	{2,  12,   IOCON_MODE_PULLDOWN | IOCON_FUNC0},	// EXT_PWR  //TODO: EINT?
	{2,  3,   IOCON_MODE_INACT | IOCON_FUNC0},	// SEL_H
	{2,  4,   IOCON_MODE_INACT | IOCON_FUNC0},	// SEL_L

	// input button
	{2,  11,   IOCON_MODE_PULLUP | IOCON_FUNC1},	// input

	// LED
	{2,  2,   IOCON_MODE_INACT | IOCON_FUNC0},	// R
	{2,  1,   IOCON_MODE_INACT | IOCON_FUNC0},	// G
	{2,  0,   IOCON_MODE_INACT | IOCON_FUNC0},	// B



};


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}
/* Setup system clocking */
STATIC void SystemSetupClocking(void)
{
  /* CPU clock source starts with IRC */
  Chip_Clock_SetMainPLLSource(SYSCTL_PLLCLKSRC_IRC);
  Chip_Clock_SetCPUClockSource(SYSCTL_CCLKSRC_SYSCLK);

  /* Enable main oscillator used for PLLs */
  LPC_SYSCTL->SCS = SYSCTL_OSCEC;
  while ((LPC_SYSCTL->SCS & SYSCTL_OSCSTAT) == 0) {}

  /* PLL0 clock source is 12MHz oscillator, PLL1 can only be the
  main oscillator */
  Chip_Clock_SetMainPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);

  /* Setup PLL0 for a 480MHz clock. It is divided by CPU Clock Divider to create CPU Clock.
  Input clock rate (FIN) is main oscillator = 12MHz
  FCCO is selected for PLL Output and it must be between 275 MHz to 550 MHz.
  FCCO = (2 * M * FIN) / N = integer multiplier of CPU Clock (120MHz) = 480MHz
  N = 1, M = 480 * 1/(2*12) = 20 */
  Chip_Clock_SetupPLL(SYSCTL_MAIN_PLL, 19, 0);/* Multiply by 20, Divide by 1 */

  /* Enable PLL0 */
  Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);

  /* Change the CPU Clock Divider setting for the operation with PLL0.
  Divide value = (480/120) = 4 */
  Chip_Clock_SetCPUClockDiv(3); /* pre-minus 1 */

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

#if 0 /* Use PLL1 output as USB Clock Source */
	/* Enable PLL1 */
	Chip_Clock_EnablePLL(SYSCTL_USB_PLL, SYSCTL_PLL_ENABLE);

	/* Wait for PLL1 to lock */
	while (!Chip_Clock_IsUSBPLLLocked()) {}

	/* Connect PLL1 */
	Chip_Clock_EnablePLL(SYSCTL_USB_PLL, SYSCTL_PLL_ENABLE | SYSCTL_PLL_CONNECT);

	/* Wait for PLL1 to be connected */
	while (!Chip_Clock_IsUSBPLLConnected()) {}
#endif

  /* Setup FLASH access to 5 clocks (120MHz clock) */
	Chip_SYSCTL_SetFLASHAccess(FLASHTIM_120MHZ_CPU);
}


/* Set up and initialize hardware prior to call to main */
void SystemInit(void)
{
	/*
	Chip_Clock_SetCrystalRangeLo();
    Chip_SetupXtalClocking();
    Chip_SYSCTL_SetFLASHAccess(FLASHTIM_100MHZ_CPU);


    Chip_Clock_SetCPUClockDiv(16);
    */

	Board_SetupMuxing();
	SystemSetupClocking();

#if defined(NO_BOARD_LIB)
	/* Chip specific SystemInit */
	//Chip_SystemInit();
#else
	/* Board specific SystemInit */
	Board_SystemInit();
#endif
}

