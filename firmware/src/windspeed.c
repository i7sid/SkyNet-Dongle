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

/*
 * you want to use this!
 */
int calcwindspeed(){
	Chip_RTC_GetFullTime(LPC_RTC, &curTimespeed);
	int curtick = 0;
	//NVIC_DisableIRQ(WINDCUPS);
	uint32_t irqmaskclear = Chip_GPIOINT_GetIntFalling(LPC_GPIOINT,GPIOINT_PORT0); // get gpio irq mask
	irqmaskclear &= ~(1 << 3); // clear bit 3
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, irqmaskclear); //disable windcup irq
	int curtimeh = timeAdd(&curTimespeed);
	int buftimeh = timeAdd(&bufferedTimespeed);
	curtimeh = timeAdd(&curTimespeed);
	curtick = cnttick;
	cnttick = 0;
	//NVIC_EnableIRQ(WINDCUPS);
	uint32_t irqmaskset = Chip_GPIOINT_GetIntFalling(LPC_GPIOINT,GPIOINT_PORT0); // get gpio irq mask
	irqmaskset |= (1 << 3); //set bit 3
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, irqmaskset); //enable windcup irq
	int div =  curtimeh -buftimeh;
	bufferedTimespeed = curTimespeed;
	speed =  curtick*(2.25/div); //mph
	speed *= 1.609344; // to kmh
	//FIXME input_windspeed_statistics(speed);
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
