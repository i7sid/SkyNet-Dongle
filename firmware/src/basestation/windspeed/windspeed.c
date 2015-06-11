/**@file
 * windspeed.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 *
 *      @brief Functionality of the Windsensor Module for Windspeed
 */

#include "windspeed.h"
#include "../../misc/misc.h"
#include "time.h"
#include "../../gpio/gpio_irq.h"

RTC_TIME_T curTimespeed;
RTC_TIME_T bufferedTimespeed;
int bufferedms;

unsigned int cnttick = 0;


/*
 * you want to use this!
 */
int calcwindspeed(){
	Chip_RTC_GetFullTime(LPC_RTC, &curTimespeed);
	int curtick = 0;
	int speed = 0;
	disable_gpio_irq(3);
	int ms = SysTick->VAL;
	long curtimeh = timeAdd(&curTimespeed,ms);
	long buftimeh = timeAdd(&bufferedTimespeed,bufferedms);
	curtick = cnttick;
	cnttick = 0;

	enable_gpio_irq(3);

	long div =  curtimeh -buftimeh;
	bufferedTimespeed = curTimespeed;
	bufferedms = ms;
	speed =  curtick*(2.25/(div/1000))*1.609344*100; //mph to km/h to 100m/h
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
	bufferedms = SysTick->VAL;
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, (1 << 3));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 3));
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);
	DBG("Initialize Wind Cups complete...\n");


}
