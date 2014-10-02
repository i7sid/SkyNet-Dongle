/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to control the on-board LED.
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

/**
 * @brief		Lights up the LED for a given amount of time.
 * @param	ms	Time in milliseconds the LED will be on.
 *
 * The specified amount of time will be waited passively,
 * so the code execution will not be delayed.
 *
 * @warning		This can lead to unexpected behaviour, if you call this
 * 				function again before the last execution was finished.
 */
void skynet_led_blink_red_passive(uint32_t ms);

///@copydoc skynet_led_blink_red_passive(uint32_t ms)
void skynet_led_blink_green_passive(uint32_t ms);

///@copydoc skynet_led_blink_red_passive(uint32_t ms)
void skynet_led_blink_blue_passive(uint32_t ms);


#endif /* LED_H_ */
