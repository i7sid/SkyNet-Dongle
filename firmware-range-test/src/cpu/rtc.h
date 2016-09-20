/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains the public interface to the Real Time Clock.
 */


#ifndef RTC_H_
#define RTC_H_

#include "../misc/misc.h"


#define RTC_ENABLED

/// @brief Memory buffer used when reading or writing the current time.
extern RTC_TIME_T FullTime;

/**
 * @brief	Initializes the Real Time Clock.
 */
void rtc_init(void);

/**
 * @brief	Prepares the RTC for CPU power down.
 *
 * e.g. disable interrupts
 */
void rtc_prepare_powerdown(void);

/**
 * @brief	Interrupt handler for Real Time Clock interrupts.
 */
void RTC_IRQHandler(void);

#endif /* RTC_H_ */
