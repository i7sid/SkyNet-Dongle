/**
 * @file
 *
 * @date	26.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Configures debugging stuff.
 */


#ifndef DEBUG_H_
#define DEBUG_H_

#if defined(SEMIHOSTING_CONSOLE)
#include <stdio.h>
#define DBG printf
#else
#define DBG
#endif


/// @brief For compatibility with legacy SkyNet code.
#define uart0_puts_P uart0_puts


#endif /* DEBUG_H_ */
