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
 * @brief 	Measure battery voltage
 * @returns	battery voltage, //TODO which format? multiplied by?
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
