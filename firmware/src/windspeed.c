/*
 * windspeed.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "windspeed.h"
#include "./misc/misc.h"
#include "time.h"


#define WINDCUPS	EINT3_IRQn

RTC_TIME_T curTimespeed;
RTC_TIME_T bufferedTimespeed;

unsigned int cnttick = 0;

int speed = 0;
int timewindow = 5; //time window for speed measurement

/*
 * you want to use this!
 */
int calcwindspeed(){
	Chip_RTC_GetFullTime(LPC_RTC, &curTimespeed);
	int curtick = 0;
	int curtimeh = timeAdd(&curTimespeed);
	int buftimeh = timeAdd(&bufferedTimespeed);
	int div =  curtimeh -buftimeh;
	if(div >= timewindow){
		NVIC_DisableIRQ(WINDCUPS);
		Chip_RTC_GetFullTime(LPC_RTC, &curTimespeed);
		curtimeh = timeAdd(&curTimespeed);
		curtick = cnttick;
		cnttick = 0;
		NVIC_EnableIRQ(WINDCUPS);
		div =  curtimeh -buftimeh;
		bufferedTimespeed = curTimespeed;
		speed =  curtick*(2.25/div); //mph
		speed *= 1.609344; // to kmh
		//input_windspeed_statistics(speed);
	}
	return speed;
}

void tickhandler(){
	//DBG("tick\n");
	cnttick++;
}

/*
 * Wind Speed translation : V = p*(2,25/t)
 * V: speed in mph
 * p: pulses per sampelperiod
 * t: sample period in sec
 */
void setupirq(){
	DBG("Initialize Wind Cups...\n");
	Chip_RTC_GetFullTime(LPC_RTC, &bufferedTimespeed);
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, (1 << 3));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 3));
	NVIC_ClearPendingIRQ(WINDCUPS);
	NVIC_EnableIRQ(WINDCUPS);
	DBG("Initialize Wind Cups complete...\n");


}
