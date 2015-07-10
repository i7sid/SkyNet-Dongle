/** @file
 *
 *  @date 09.07.2015
 *  @author Christoph Kohl
 *	@brief Configurable Values
 */

#ifndef BASESTATION_CONFIG_H_
#define BASESTATION_CONFIG_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

bool skipcompasscal;
bool skipgpsfix;
int magneticdeclination;
char * deviceID;
unsigned int bufferedtime;
unsigned int sendtime;

#endif /* BASESTATION_CONFIG_H_ */
