/*
 * bitprint.h
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#ifndef BITPRINT_H_
#define BITPRINT_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

void printBin32littleE(uint32_t n);

void printBin32(uint32_t n);

void printBin16littleE(uint16_t n);

void printBin16(uint16_t n);

int castuint16toint(uint16_t n);

#endif /* BITPRINT_H_ */
