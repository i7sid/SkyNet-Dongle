/*
 * compass.h
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#ifndef COMPASS_H_
#define COMPASS_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

void calibcompass();

void setupcompass();

double readCompass();

#endif /* COMPASS_H_ */
