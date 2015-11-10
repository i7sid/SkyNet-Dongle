/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */



#include "../periph/adc.h"
#include "../misc/misc.h"
#include "../periph/adc.h"
#include "../gpio//gpio_irq.h"
#include "../periph/led.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>


#include "compass/compass.h"
#include "gps/gps.h"
#include "wind/windspeed/windspeed.h"
#include "wind/windvane/windvane.h"


void skynetbase_init(void) {
	DBG("Init Basestation...\n");

	int r = 0;

	r = skynetbase_compass_init();
	if (!r) {
		// TODO error: no compass
		return;
	}

	r = skynetbase_windvane_init();
	if (!r) {
		// TODO error: no windvane
		return;
	}

	r = skynetbase_windspeed_init();
	if (!r) {
		// TODO error: no windspeed
		return;
	}

	r = skynetbase_gps_init();
	if (!r) {
		// TODO error: no gps
		return;
	}

}

void skynetbase_start_measurement(void) {
	// TODO
}

void skynetbase_stop_measurement(void){
	// TODO
}


void basedata_measure_start(){
	//register_delayed_event(updaterate, (void*) updateBaseData);
}

void basedata_measure_stop(){
	//remove_delayed_event((void*) updateBaseData);
}

