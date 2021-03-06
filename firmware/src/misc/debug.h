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
#elif defined(DEBUG_USB)
#include <stdio.h>
#define DBG skynet_cdc_write_debug
#else
static inline void do_nothing_debug(char* fmt, ...) { return; };
#define DBG do_nothing_debug
#endif


/// @brief For compatibility with legacy SkyNet code.
#define uart0_puts_P uart0_puts


#ifdef CORE_M3
extern uint32_t sn_cnt_mallocs;
extern uint32_t sn_cnt_frees;
void malloc_count(void);
void free_count(void);
#else
inline void malloc_count(void) {}
inline void free_count(void) {}
#endif


#endif /* DEBUG_H_ */
