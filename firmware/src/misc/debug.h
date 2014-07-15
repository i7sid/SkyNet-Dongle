/**
 * @file
 *
 * @date	26.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#ifndef DEBUG_H_
#define DEBUG_H_

#if defined(SEMIHOSTING_CONSOLE)
#include <stdio.h>
#define DBG printf
#else
#define DBG
#endif


#define uart0_puts_P uart0_puts

/*
INLINE void uart0_puts_P(char* str) {
	DBG(str);
}

INLINE void uart0_puts(char* str) {
	DBG(str);
}

INLINE void uart0_putc(char c) {
	char str[2];
	str[0] = c;
	str[1] = '\0';
	DBG(str);
}
*/
#endif /* DEBUG_H_ */
