/*
 * winddirection.h
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#ifndef WINDDIRECTION_H_
#define WINDDIRECTION_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

int getWindDirection();

void setupadc();


#endif /* WINDDIRECTION_H_ */
