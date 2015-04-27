/*
 * windspeed.h
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#ifndef WINDSPEED_H_
#define WINDSPEED_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

int calcwindspeed();

void tickhandler();

void setupirq();


#endif /* WINDSPEED_H_ */
