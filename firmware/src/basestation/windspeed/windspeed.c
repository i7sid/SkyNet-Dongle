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

static uint32_t ticksPerSecond;

unsigned int cnttick = 0;


/*
 * you want to use this!
 */
int calcwindspeed(){

	int curtick = 0;
	int speed = 0;
	disable_gpio_irq(2,13);


	curtick = cnttick;
	cnttick = 0;
	float div = Chip_TIMER_ReadCount(LPC_TIMER2)/ticksPerSecond;

	Chip_TIMER_Reset(LPC_TIMER2);
	enable_gpio_irq(2,13);
	DBG("div:%f count:%d\n",div,curtick);

	speed =  curtick*(2.25/(div))*1.609344*100; //mph to km/h to 100m/h
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

	Chip_GPIOINT_Init(LPC_GPIOINT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO,2,13);
	Chip_GPIO_SetPinState(LPC_GPIO, 2, 13, true);

	Chip_IOCON_PinMux(LPC_IOCON, 2, 13, IOCON_MODE_PULLUP, IOCON_FUNC0);

	const uint32_t prescaleDivisor = 8;
	Chip_TIMER_Init(LPC_TIMER2);
	Chip_TIMER_PrescaleSet(LPC_TIMER2, prescaleDivisor - 1);
	Chip_TIMER_Enable(LPC_TIMER2);
	ticksPerSecond = Chip_Clock_GetSystemClockRate() / prescaleDivisor / 4;

	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, (1 << 13));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, (1 << 13));

	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);
	DBG("Initialize Wind Cups complete...\n");


}
