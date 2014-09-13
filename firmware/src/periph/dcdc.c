/**
 * @file
 *
 * @date	08.09.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality to control the Buck-Boost-Converter
 */

#include "dcdc.h"

void dcdc_init(void) {
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DCDC_PS_PORT, DCDC_PS_PIN);
	dcdc_set_powersave(false);
}

INLINE void dcdc_set_powersave(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, DCDC_PS_PORT, DCDC_PS_PIN, !state);
}
