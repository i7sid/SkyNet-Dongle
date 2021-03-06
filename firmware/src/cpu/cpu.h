/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains the public interface to CPU power management.
 */



#ifndef PM_H_
#define PM_H_

#include "../misc/misc.h"

typedef enum cpu_speed {
	SPEED_FULL,
	SPEED_120MHz,
	SPEED_60MHz,
	SPEED_30MHz,
	SPEED_7M5Hz
} cpu_speed;

extern volatile bool cpu_powered_down;

/**
 * @brief			Sets the CPU speed to save energy or provide full power.
 * @param	speed	target CPU speed
 */
void cpu_set_speed(cpu_speed speed);

/**
 * @brief			Sends the CPU to a sleep state waiting for interrupts.
 */
void cpu_sleep();


/**
 * @brief			Resets the node.
 */
void cpu_reset();

/**
 * @brief			Resets the cpu to enter bootloader mode.
 *
 * Starts a USB mass storage device to transfer new firmware.
 */
void cpu_enter_iap_mode();


#endif /* PM_H_ */
