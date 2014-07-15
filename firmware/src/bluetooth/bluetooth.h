/**
 * @file
 *
 * @date	25.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define BLUETOOTH_UART_INT_MASK 	(UART_IER_RBRINT /*| UART_IER_RLSINT*/)

#include "../misc/misc.h"

/**
 * @brief	Initializes chip pins and UART.
 */
void bt_init();

/**
 * @brief			Initialize UART with specified speed.
 * @param	baud	Baud rate
 */
void bt_uart_init(uint32_t baud);

/**
 * @brief	Deinitializes the bluetooth UART.
 */
void bt_uart_deinit();

/**
 * @brief 	Check if bluetooth module works at full speed.
 *
 * If not, start initial configuration routine and configure bluetooth module
 * to work with a baud rate if 115200.
 */
void bt_first_configuration();

/**
 * @brief	Deinitializes chip pins and UART.
 *
 * @note	This also calls bt_uart_int_disable() .
 */
void bt_deinit();

/**
 * @brief	Resets the bluetooth module by sending AT+RESET\\r\\n
 */
void bt_reset();

/**
 * @brief	Resets the bluetooth module by pulling RESET \b low.
 */
void bt_hardreset();

/**
 * @brief	Registers and enables UART interrupt.
 */
INLINE void bt_uart_int_enable();

/**
 * @brief	Disables UART interrupt.
 */
INLINE void bt_uart_int_disable();

/**
 * @brief 	Take bluetooth module into AT command mode
 */
void bt_enable_AT_mode();

/**
 * @brief	Stop AT command mode and enable data transmission.
 *
 * @info 	This pulls the AT pin \b low and sends AT+RESET\\r\\n.
 * 			Without soft reset HC-06 does not leave AT mode.
 */
void bt_disable_AT_mode();

/**
 * @brief	Shutdown bluetooth module by pulling RESET.
 */
void bt_shutdown();

/**
 * @brief	Wake up bluetooth module by stop pulling RESET.
 */
void bt_wakeup();

/**
 * @brief	Send's the AT command to make the bluetooth module visible to clients.
 */
void bt_make_visible();

/**
 * @brief	Send's the AT command to make the bluetooth module invisible to clients.
 */
void bt_make_invisible();

/**
 * @brief			Makes the module visible or invisible to clients.
 * @param	visible	\b true for visible, \b false for invisible
 */
STATIC INLINE void bt_change_visible(bool visible);

/**
 * @brief 		Print null-terminated string to UART.
 * @param	str	null-terminated string.
 */
INLINE void bt_uart_puts(char *str);

/**
 * @brief 			Print count characters from str to UART.
 * @param	str		char array, needs not to be terminated
 * @param	count	number of characters from str
 *
 * @warning			Be aware of buffer overflows!
 */
INLINE void bt_uart_nputs(char *str, int count);

/**
 * @brief 		Print single character to UART.
 * @param	chr	Character to print to UART.
 */
INLINE void bt_uart_putc(char chr);

/**
 * @brief			Helper function that toggles the AT pin.
 * @param	state	\b true for AT mode, \b false for data mode
 */
STATIC INLINE void bt_switch_at_pin(bool state);

/**
 * @brief			Helper function that toggles the RESET pin.
 * @param	state	\b true for RESET, \b false for normal operation
 *
 * If RESET is set, bluetooth module sleeps with nearly no current consumption.
 */
STATIC INLINE void bt_switch_reset_pin(bool state);

/**
 * @brief 			Clears the RX buffer removing all remaining bytes.
 */
INLINE void bt_uart_clear_rx();

#define bt_uart_puts_P bt_uart_puts

static void bt_do_read(int len, char* data);



#endif /* BLUETOOTH_H_ */
