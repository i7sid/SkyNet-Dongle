/**
 * @file
 *
 * @date	10.05.2016
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief
 */


#include "v_storage.h"

V_DATA_T v_configuration;

void skynet_v_init(void) {
	v_configuration.time_pos_wait = 20000;
	v_configuration.time_wind_wait = 1000;
}
