/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef INPUT_H_
#define INPUT_H_

#include "../misc/misc.h"


/**
 * @brief 	Amount of that is waited between to checks
 *
 * A higher value leads to better performs (less checks and thus cpu cycles),
 * but also increases the possibility to lose a \b HIGH / \b LOW change.
 */
#define SWITCH_CHECK_WAIT_MS		(5)

/// @brief	Amount of time the switch has to be pressed to be recognized.
#define SWITCH_PRESS_SINGLE_MS		(15)

/// @brief	Maximum amount of time between to pressed to be recognized as a double press.
#define SWITCH_PRESS_DOUBLE_MS 		(500)

/// @brief	Minimum amount of time the switch has to be pressed for POWER function.
#define SWITCH_PRESS_POWER_MS 		(2000)

/// @brief	Calculated amount of times the checks have to be repeated.
#define SWITCH_PRESS_SINGLE_TIMES	(SWITCH_PRESS_SINGLE_MS / SWITCH_CHECK_WAIT_MS)
/// @copydoc SWITCH_PRESS_SINGLE_TIMES
#define SWITCH_PRESS_DOUBLE_TIMES	(SWITCH_PRESS_DOUBLE_MS / SWITCH_CHECK_WAIT_MS)
/// @copydoc SWITCH_PRESS_SINGLE_TIMES
#define SWITCH_PRESS_POWER_TIMES	(SWITCH_PRESS_POWER_MS / SWITCH_CHECK_WAIT_MS)


/**
 * @brief  Defines which kind of switch press event occurred.
 */
typedef enum switch_pressed_type {
	SWITCH_SHORT, 	///< @brief short single press
	SWITCH_POWER, 	///< @brief long single press (min. 2 sec)
	SWITCH_DOUBLE	///< @brief double click
} switch_pressed_type;

/**
 * @brief	Defines in which state the state machine of switch control is.
 */
typedef enum switch_check_state {
	SWITCH_FSM_NOT_PRESSED,		///< @brief Switch was not pressed
	SWITCH_FSM_FIRST,			///< @brief Switch is pressed for the first time
	SWITCH_FSM_SECOND_WAITING,	///< @brief Waiting if switch is pressed twice...
	SWITCH_FSM_SECOND			///< @brief Switch is pressed for the second time
} switch_check_state;


/**
 * @brief	Initialize the switch input (GPIO, interrupt, ...)
 */
void input_init(void);

/**
 * @brief	Get the current state of the switch.
 *
 * @return	\b true for \"switch pressed\", \b false otherwise
 */
bool input_state(void);

/**
 * @brief	Event handler function that is called when the switch was pressed.
 * @param	type	How the switch was pressed.
 */
void input_pressed(switch_pressed_type type);

/**
 * @brief Interrupt Service Routine that handles initial button press.
 */
void INPUT_SWITCH_HANDLER(void);

#endif /* INPUT_H_ */
