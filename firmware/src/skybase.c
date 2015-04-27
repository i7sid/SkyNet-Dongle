/*
 * skybase.c
 *
 *  Created on: 27.02.2015
 *      Author: christoph
 */
#include "skybase.h"
#include "./periph/adc.h"
#include "./misc/misc.h"
#include "./periph/adc.h"
#include "./radio/skynet_radio.h"
#include "./gpio_irq.h"
#include "./bitprint.h"
#include "./windspeedstatistics.h"
#include "./winddirectionstatistics.h"
#include "winddirection.h"
#include "time.h"
#include "compass.h"
#include "windspeed.h"
#include <math.h>

bool compassconnected = false;


void baseinit(){
	DBG("Initialize Basestation Unit...\n");
	setupadc();
	setupirq();

		if (compassconnected){ //TODO use cpp
			setupcompass();
			calibcompass();
		}
	//	init_windspeed_statistics();
	//	init_winddir_statistics();
	DBG("Initialize Basestation Unit complete...\n");
}

void updateBaseData(){
	int dir = getWindDirection();
	//double dirMN = 480;
	if (compassconnected){
		//dirMN = readCompass();
		//TODO compass correction of wind direction
	}
	int speed = calcwindspeed();
	DBG("Basedata:\n Winddirection: %d deg; "
			"Windspeed: %d kmh \n", dir, speed);
}
