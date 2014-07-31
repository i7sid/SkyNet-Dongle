/**
 * @file
 *
 * @date	28.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "adc.h"

static ADC_CLOCK_SETUP_T ADCSetup;

void adc_init() {
	// init pwr pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, ADC_PWR_PORT, ADC_PWR_PIN);

	// init ADC
	Chip_ADC_Init(LPC_ADC, &ADCSetup);

	adc_deactivate();
}

uint16_t adc_measure() {
	uint16_t data;

	// activate
	adc_activate();

	// Waiting for A/D conversion complete
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {}

	// Read ADC value
	Chip_ADC_ReadValue(LPC_ADC, ADC_CHANNEL, &data);

	// deactivate
	adc_deactivate();

	return data; // TODO: multiply by 2 to because of voltage divider ?
}


INLINE void adc_activate() {
	PINSET(ADC_PWR_PORT, ADC_PWR_PIN);
	msDelay(50);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

INLINE void adc_deactivate() {
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, DISABLE);
	PINCLR(ADC_PWR_PORT, ADC_PWR_PIN);
}
