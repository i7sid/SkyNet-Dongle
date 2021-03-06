/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "windspeed.h"

static uint32_t last_tick = 0;
static float current_speed = 0;
/*static*/ float current_speed_2 = 0;
static float last_checked_speed = 0;
static uint8_t null_ticks = 0;
static uint16_t ticks_2 = 0;

float skynetbase_windspeed_get(void) {
	return current_speed;
}

__INLINE void skynetbase_windspeed_tickhandler() {
	// measure time between two impulses
	ticks_2++;

	__disable_irq();
	uint32_t now = skynet_systick_get();
	__enable_irq();

	if (last_tick > 0) {
		uint32_t diff = now - last_tick;
		float f = (float)1000.0 / (float)diff;
		current_speed = 2.25 * 1.609344 * f;
	}

	last_tick = now;
}

void skynetbase_windspeed_check_null(void) {
	if (current_speed == last_checked_speed) {
		null_ticks++;
	}
	else {
		null_ticks = 0;
	}

	if (null_ticks >= 4) {
		current_speed = 0;
		null_ticks = 0;
	}

	last_checked_speed = current_speed;
	register_delayed_event(1000, skynetbase_windspeed_check_null);

	// try first method
	current_speed_2 = 2.25 * 1.609344 * ticks_2;
	ticks_2 = 0;
}

int skynetbase_windspeed_init(void) {
	DBG("Initialize Wind Cups...\n");

	Chip_GPIOINT_Init(LPC_GPIOINT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, WINCUPS_PORT, WINCUPS_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, WINCUPS_PORT, WINCUPS_PIN, true);

	Chip_IOCON_PinMux(LPC_IOCON, WINCUPS_PORT, WINCUPS_PIN, IOCON_MODE_PULLUP, IOCON_FUNC0);

	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, (1 << WINCUPS_PIN));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, (1 << WINCUPS_PIN));

	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);

	DBG("Initialize Wind Cups complete.\n");

	register_delayed_event(1000, skynetbase_windspeed_check_null);
	return true;
}
