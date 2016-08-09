/**
 *  @file
 *
 *  @date 	08.08.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_HISTORY_H_
#define BASESTATION_HISTORY_H_

#include "chip.h"

#define HISTORY_CHANNEL_COUNT	(4)

#define HISTORY_WIND_SPEED_SHORT	(0)
#define HISTORY_WIND_SPEED_LONG		(1)
#define HISTORY_WIND_DIR_SHORT		(2)
#define HISTORY_WIND_DIR_LONG		(3)


float history_new_sample(uint8_t channel, float value);
float history_avg_get(uint8_t channel);
void history_set_weight(uint8_t channel, float weight);



#endif /* BASESTATION_HISTORY_H_ */
