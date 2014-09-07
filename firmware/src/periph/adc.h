/**
 * @file
 *
 * @date	28.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef ADC_H_
#define ADC_H_

#include "../misc/misc.h"
#include "../cpu/systick.h"


/**
 * @brief 	Initialize ADC unit.
 */
void adc_init(void);

/**
 * @brief	Discards all operation on ADC to reduce power consumption.
 */
void adc_deinit(void);

/**
 * @brief	Start regular update of buffered ADC measurement.
 */
void adc_start_buffered_measure(void);

/**
 * @brief	Callback for delayed event for ADC measurement.
 */
void adc_read_buffered_measure(void);

/**
 * @brief	Disables the regular update of buffered ADC measurements.
 *
 * Needed for example if you want to go to deep sleep or simply
 * do not need a current battery voltage measurement.
 */
void adc_stop_buffered_measure(void);

/**
 * @brief	Get latest value from buffered ADC measurement.
 * @returns	Last value that was read from via \ref adc_measure() .
 */
uint16_t adc_get_buffered_value(void);

/**
 * @brief 	Measure battery voltage
 * @returns	battery voltage, 12 bit
 */
uint16_t adc_measure(void);

/**
 * @brief Activate ADC measurement circuit.
 *
 * Switches the MOSFETs so that the measurement can be done.
 * In this state the measurement circuit consumes current.
 */
void adc_activate(void);

/**
 * @brief Deactivate ADC measurement circuit.
 *
 * Switches the MOSFETs so that the measurement can \b not be done.
 * In this state the measurement circuit consumes \b no current.
 */
void adc_deactivate(void);

#endif /* ADC_H_ */
