/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_WIND_WINDSPEED_WINDSPEED_H_
#define BASESTATION_WIND_WINDSPEED_WINDSPEED_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "../../../misc/misc.h"
#include "../../../misc/debug.h"
#include "../../../gpio/gpio_irq.h"
#include "../../../cpu/systick.h"

/**
 * @brief	Initializes the windspeed measurement unit.
 */
int skynetbase_windspeed_init(void);

/**
 * @brief	Called in GPIO ISR to count rotations.
 */
void skynetbase_windspeed_tickhandler(void);



#endif /* BASESTATION_WIND_WINDSPEED_WINDSPEED_H_ */
