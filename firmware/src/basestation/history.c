/**
 *  @file
 *
 *  @date 	08.08.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "history.h"

static float values[HISTORY_CHANNEL_COUNT];
static float weights[HISTORY_CHANNEL_COUNT];

float history_new_sample(uint8_t channel, float value) {
	values[channel] = weights[channel] * value + (1 - weights[channel]) * values[channel];
	return values[channel];
}

float history_avg_get(uint8_t channel) {
	return values[channel];
}

void history_set_weight(uint8_t channel, float weight) {
	weights[channel] = weight;
}

