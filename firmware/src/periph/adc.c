/**
 * @file
 *
 * @date	28.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for analog digital conversion, e.g. battery monitoring.
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "adc.h"
#include "../cpu/systick.h"

/// @brief Buffer for holding setup data returned from Chip_ADC_Init().
static ADC_CLOCK_SETUP_T ADCSetup;

/// @brief Last value that was read from ADC in buffered measure mode.
static volatile uint16_t adc_buffered_value = 0xFFF; // 4095


void adc_init(void) {
	// init pwr pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, ADC_PWR_PORT, ADC_PWR_PIN);

	// init ADC
	Chip_ADC_Init(LPC_ADC, &ADCSetup);

	adc_deactivate();
}

void adc_deinit(void) {
	adc_stop_buffered_measure();
	adc_deactivate();
	Chip_ADC_DeInit(LPC_ADC);
	//TODO: Preiph Clock trennen möglich?
}

void adc_start_buffered_measure(void) {
	register_delayed_event(5000, adc_read_buffered_measure);
}

void adc_read_buffered_measure(void) {
	adc_buffered_value = adc_measure();
	adc_start_buffered_measure();
}

INLINE uint16_t adc_get_buffered_value(void) {
	return adc_buffered_value;
}

INLINE void adc_stop_buffered_measure(void) {
	remove_delayed_event(adc_read_buffered_measure);
}

uint16_t adc_measure(void) {
	uint16_t data;

	// activate
	adc_activate();

	// Waiting for A/D conversion complete
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {}

	// Read ADC value
	Chip_ADC_ReadValue(LPC_ADC, ADC_CHANNEL, &data);

	// deactivate
	adc_deactivate();

	return data;
}


INLINE void adc_activate(void) {
	PINSET(ADC_PWR_PORT, ADC_PWR_PIN);
	msDelayActive(50);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

INLINE void adc_deactivate(void) {
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, DISABLE);
	PINCLR(ADC_PWR_PORT, ADC_PWR_PIN);
}
