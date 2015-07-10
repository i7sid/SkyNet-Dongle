/*
 * databuffer.h
 *
 *  Created on: 20.05.2015
 *      Author: christoph
 */

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

bool putdata(int speed, int dir);

void readdata(unsigned int time , int * speed, int * dir);

bool putdir(int val);

bool putspeed(int val);

int readspeedbuffer(unsigned int time);

int readdirbuffer(unsigned int time);


#endif /* DATABUFFER_H_ */
