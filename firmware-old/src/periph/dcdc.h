/**
 * @file
 *
 * @date	08.09.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to control the Buck-Boost-Converter
 */

#ifndef DCDC_H_
#define DCDC_H_

#include "../misc/misc.h"


/**
 * @brief	Initialize the needed port pins.
 */
void dcdc_init(void);

/**
 * @brief			Activate or deactivate power save mode.
 * @param	state	\b true: Power Save mode enabled; \b false: normal operation mode
 *
 * In Power Save mode, the TPS63001 internally switches of some circuits to
 * enable a very low power consumption. Only do this, if the device needs
 * w \b really little current, for example in power down mode.
 */
void dcdc_set_powersave(bool state);

#endif /* DCDC_H_ */
