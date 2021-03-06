/**@file
 * winddirection.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 *      @brief Functionality of the Windsensor Module for Winddirection
 */

#include "../../basestation_old/winddirection/winddirection.h"

#include "../../misc/misc.h"
#include "../../periph/adc.h"
#include "time.h"
#include "../../periph/adc.h"
#include "stdlib.h"
#include "../../basestation_old/compass/compass.h"
#include "../../basestation_old/config.h"
#include "../../basestation_old/tools/mutex.h"
#include "../../basestation_old/tools/statistics_tools.h"
#include "../../basestation_old/winddirection/winddirectionstatistics.h"

#define WINDVANE	ADC_CH6



int buffcount = 0;
//uint16_t bufferdir[buffersize];
uint16_t * bufferdir;


/*
 * you want to use this!
 */

int getWindDirection(){
	//measure stop();
	winvane_measure_stop();
	int dir = 0;
	for (int i = 0; i < buffersize; i++){
		dir = (addvec2(dir,(int)(((float)bufferdir[i])/11.37)));
	}
	//measure start
	winvane_measure_start();
	float corr = readCompass();
	dir -= corr;
	return dir;

}

int getWindDirection_raw(){
	//measure stop();
	winvane_measure_stop();
	float dir = 0;
	for (int i = 0; i < buffersize; i++){
		dir = (addvec2(dir,(int)(((float)bufferdir[i])/11.37)));
	}
	//measure start
	winvane_measure_start();
	return (int)dir;

}

void winvane_measure_start(){
	register_delayed_event(samplerate, (void*) winvane_measure);
}

void winvane_measure_stop(){
	remove_delayed_event((void*) winvane_measure);
}

void winvane_measure(){

	// activate adc
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, DISABLE);//disable charger channel
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	// Waiting for A/D conversion complete
	while (Chip_ADC_ReadStatus(LPC_ADC, WINDVANE, ADC_DR_DONE_STAT) != SET) {
		//DBG("Waiting for ADC val \n");
	}

	// Read ADC value
	Chip_ADC_ReadValue(LPC_ADC, WINDVANE,&bufferdir[buffcount] );

	// deactivate
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE, DISABLE);

	buffcount++;
	if(buffcount == buffersize){
		buffcount = 0;
	}
	winvane_measure_start();
}

/*
 * Wind Direction translation: 0-20kOhm, 10kOhm = 180deg = south
 *
 */
void setupadc(){
	DBG("Initialize Wind Vane...\n");
	bufferdir = malloc(buffersize*sizeof(uint16_t));
	if (bufferdir == NULL){
		//fixme error
	}
	Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC2);
	winvane_measure_start();
	DBG("Initialize Wind Vane complete...\n");
}

