/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to charger configuration.
 */

#ifndef CHARGER_H_
#define CHARGER_H_

#include "../misc/misc.h"

/**
 * @brief	Defines the mode the charge controller can charge the battery.
 */
typedef enum charge_mode {
	USB_LOW, 	///< @brief max 0.1A
	USB_HIGH,	///< @brief max 0.5A
	USB_CHARGE	///< @brief max 1.8A
} charge_mode;

/**
 * @brief	Initializes the pins the charger is attached to.
 * This functionality is for a MCP73871.
 */
void charger_init(void);

/**
 * @brief	Returns the value of STAT1.
 *
 * Basically this is \b low / \b false if battery is charging or an error occured.
 * For exact status see datasheet: http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en536670
 *
 * @returns	\b true: Shutdown, Charge Complete, No Battery Present, No Input Power Present;
 * 			\b false: Charging, Fault
 */
bool charger_status1(void);

/**
 * @brief	Returns the value of STAT2
 *
 * Basically this is \b low / \b false if charging completed or an error occured.
 * For exact status see datasheet: http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en536670
 *
 * @returns	\b true: Shutdown, Charging, No Battery Present
 * 			\b false: Charge Complete, Fault
 */
bool charger_status2(void);

/**
 * @brief	Returns if an external power supply is attached.
 * @returns	\b true if external power supply is attached, \b false otherwise.
 */
bool charger_external_power(void);

/**
 * @brief			Sets the maximum current the battery is charged with.
 * @param	mode	maximum current, see \ref charge_mode
 */
void charger_set_mode(charge_mode mode);

/**
 * @brief	Checks if a USB cable is connected and if it is a charging port.
 */
void charger_check_port(void);

/**
 * @brief	Interrupt handler
 */
void CHARGER_EXTPWR_HANDLER(void);

#endif // CHARGER_H_
