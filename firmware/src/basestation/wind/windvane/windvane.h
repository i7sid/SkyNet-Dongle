/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_WIND_WINDVANE_WINDVANE_H_
#define BASESTATION_WIND_WINDVANE_WINDVANE_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "../../../misc/debug.h"
#include "../../../misc/misc.h"

#define WINDVANE_ADC		ADC_CH6



/**
 * @brief	Initializes the wind direction measurement unit.
 */
int skynetbase_windvane_init(void);


void skynetbase_windvane_start(void);

void skynetbase_windvane_stop(void);

uint16_t skynetbase_windvane_measure(void);


#endif /* BASESTATION_WIND_WINDVANE_WINDVANE_H_ */
