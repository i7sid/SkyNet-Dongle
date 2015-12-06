/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "windvane.h"

uint16_t windvane_buffer;
uint16_t windvane_max = 0x1000;		// 4096
uint16_t windvane_max_target = 360;

int skynetbase_windvane_init(void) {
	DBG("Initialize Wind Vane...\n");
	Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC2);

	skynetbase_windvane_start();
	DBG("Initialize Wind Vane complete.\n");
	return true;
}

__INLINE void skynetbase_windvane_start(void) {
	// activate adc
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, DISABLE);
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE_ADC, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

__INLINE void skynetbase_windvane_stop(void) {
	// deactivate
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE_ADC, DISABLE);
}

uint16_t skynetbase_windvane_measure(void) {
	skynetbase_windvane_start();

	// Waiting for A/D conversion complete
	while (Chip_ADC_ReadStatus(LPC_ADC, WINDVANE_ADC, ADC_DR_DONE_STAT) != SET) {}

	// Read ADC value
	Chip_ADC_ReadValue(LPC_ADC, WINDVANE_ADC, &windvane_buffer);

	skynetbase_windvane_stop();

	uint32_t upscaled = (uint32_t)windvane_buffer * (uint32_t)windvane_max_target;

	return upscaled / windvane_max;
}
