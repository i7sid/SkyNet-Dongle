/*
 * skybase.h
 *
 *  Created on: 16.01.2015
 *      Author: christoph
 */

#ifndef SKYBASE_H_
#define SKYBASE_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

void updateBaseData();

void baseinit();

#endif /* SKYBASE_H_ */
