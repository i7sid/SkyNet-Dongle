/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef LED_H_
#define LED_H_

#include "../misc/misc.h"

/**
 * @brief Initializes the LED pins
 */
void skynet_led_init();

/**
 * @brief 		Switches LED on or off.
 * @param 	on	\b true for ON, \b false for \b OFF
 */
void skynet_led_red(bool on);

///@copydoc led_red(bool on)
void skynet_led_green(bool on);

///@copydoc led_red(bool on)
void skynet_led_blue(bool on);


//TODO: Blinken mittels delayed_event

#endif /* LED_H_ */
