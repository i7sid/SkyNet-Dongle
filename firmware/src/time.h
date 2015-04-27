/*
 * time.h
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#ifndef TIME_H_
#define TIME_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

int timeAdd(RTC_TIME_T* time);

#endif /* TIME_H_ */
