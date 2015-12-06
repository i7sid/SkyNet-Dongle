/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "windspeed.h"

static uint32_t ticksPerSecond;
static uint32_t tick_counter = 0;

static float current_speed = 0;

#define WAY_PER_ROTATION	(0.37699112f)	// 2 * 6cm * PI

static void measurement(void) {
	volatile uint32_t curtick;	// temporary buffer

	disable_gpio_irq(WINCUPS_PORT, WINCUPS_PIN);	// disable counting of new ticks for a moment
	curtick = tick_counter;
	tick_counter = 0;
	enable_gpio_irq(WINCUPS_PORT, WINCUPS_PIN);		// we can enable it again

	float div = (float)Chip_TIMER_ReadCount(LPC_TIMER2) / (float)ticksPerSecond;
	Chip_TIMER_Reset(LPC_TIMER2);


	// convert mph to km/h  (derived from original work)
	current_speed = (float)curtick * (2.25 / (div)) * 1.609344;

	// alternative approach?
	//current_speed = (WAY_PER_ROTATION * (float)curtick) / div;

	DBG("div:%4.4f; count:%d; speed:%4.4f\n", div, curtick, current_speed);

	register_delayed_event(1000, measurement); // enqueue next measurement
}

__INLINE void skynetbase_windspeed_tickhandler(){
	tick_counter++;
}

int skynetbase_windspeed_init(void) {
	DBG("Initialize Wind Cups...\n");

	Chip_GPIOINT_Init(LPC_GPIOINT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, WINCUPS_PORT, WINCUPS_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, WINCUPS_PORT, WINCUPS_PIN, true);

	Chip_IOCON_PinMux(LPC_IOCON, WINCUPS_PORT, WINCUPS_PIN, IOCON_MODE_PULLUP, IOCON_FUNC0);

	const uint32_t prescaleDivisor = 8;
	Chip_TIMER_Init(LPC_TIMER2);
	Chip_TIMER_PrescaleSet(LPC_TIMER2, prescaleDivisor - 1);
	Chip_TIMER_Enable(LPC_TIMER2);
	ticksPerSecond = Chip_Clock_GetSystemClockRate() / prescaleDivisor / 4;

	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, (1 << WINCUPS_PIN));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, (1 << WINCUPS_PIN));

	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);
	DBG("Initialize Wind Cups complete.\n");

	register_delayed_event(1000, measurement);

	return true;
}
