/**
 * @file
 *
 * @date	25.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to bluetooth module.
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "../misc/misc.h"


/**
 * @brief	Bluetooth enabled interrupt mask
 *
 * \b UART_IER_RBRINT   for Data Interrupts
 * \b UART_IER_RLSINT	for Line Status Interrupts
 *
 * for example to enable both: UART_IER_RBRINT | UART_IER_RLSINT
 */
#define BLUETOOTH_UART_INT_MASK 	(UART_IER_RBRINT)

/// @brief 	Milliseconds waited \b before a AT/data mode switch is performed
#define BLUETOOTH_AT_PRESWITCH_DELAY	(100)

/// @brief 	Milliseconds waited \b after a AT/data mode switch is performed
#define BLUETOOTH_AT_POSTSWITCH_DELAY	(250)

/**
 * @brief 	Milliseconds waited \b before module is shut down
 *
 * Needed to assure that all previously sent commands are executed and
 * all previously sent data is processed/transferred.
 */
#define BLUETOOTH_BEFORE_SHUTDOWN_DELAY	(100)


/**
 * @brief 	Milliseconds waited \b after module is shut down
 *
 * Assure that module is fully shutdown and all registers are empty
 * to provide a fully clean startup next time (that can be possibly right
 * \b now after the post-shutdown delay).
 */
#define BLUETOOTH_AFTER_SHUTDOWN_DELAY	(1500)


/**
 * @brief 	Milliseconds waited \b after module is woken up
 *
 * Assure that module is fully up and running, for example to transmit data
 * or to execute an AT command.
 */
#define BLUETOOTH_AFTER_WAKEUP_DELAY	(1500)

/// @brief	Packet reception buffer for bluetooth
extern uint8_t bt_packet_rx_buf[BLUETOOTH_BUFFER_SIZE+1];


/**
 * @brief	Initializes chip pins and UART.
 */
void bt_init(void);

/**
 * @brief			Initialize UART with specified speed.
 * @param	baud	Baud rate
 */
void bt_uart_init(uint32_t baud);

/**
 * @brief			Set UART to specified speed.
 * @param	baud	Baud rate
 */
void bt_uart_set_speed(uint32_t baud);

/**
 * @brief	Deinitializes the bluetooth UART.
 */
void bt_uart_deinit(void);

/**
 * @brief 	Check if bluetooth module works at full speed.
 *
 * If not, start initial configuration routine and configure bluetooth module
 * to work with a baud rate if 115200.
 */
void bt_first_configuration(void);

/**
 * @brief	Deinitializes chip pins and UART.
 *
 * @note	This also calls bt_uart_int_disable() .
 */
void bt_deinit(void);

/**
 * @brief	Resets the bluetooth module by sending AT+RESET\\r\\n
 */
void bt_reset(void);

/**
 * @brief	Resets the bluetooth module by pulling RESET \b low.
 */
void bt_hardreset(void);

/**
 * @brief	Resets the bluetooth module by sending AT+RESET\\r\\n.
 */
void bt_softreset(void);

/**
 * @brief	Registers and enables UART interrupt.
 */
void bt_uart_int_enable(void);

/**
 * @brief	Disables UART interrupt.
 */
void bt_uart_int_disable(void);

/**
 * @brief 	Take bluetooth module into AT command mode
 */
void bt_enable_AT_mode(void);

/**
 * @brief	Stop AT command mode and enable data transmission.
 *
 * @info 	This pulls the AT pin \b low and sends AT+RESET\\r\\n.
 * 			Without soft reset HC-06 does not leave AT mode.
 */
void bt_disable_AT_mode(void);

/**
 * @brief	Shutdown bluetooth module by pulling RESET.
 */
void bt_shutdown(void);

/**
 * @brief	Wake up bluetooth module by stop pulling RESET.
 */
void bt_wakeup();

/**
 * @brief	Send's the AT command to make the bluetooth module visible to clients.
 */
void bt_make_visible(void);

/**
 * @brief	Send's the AT command to make the bluetooth module invisible to clients.
 */
void bt_make_invisible(void);

/**
 * @brief			Makes the module visible or invisible to clients.
 * @param	visible	\b true for visible, \b false for invisible
 */
void bt_change_visible(bool visible);

/**
 * @brief 		Print null-terminated string to UART.
 * @param	str	null-terminated string.
 */
void bt_uart_puts(char *str);

/**
 * @brief 			Print count characters from str to UART.
 * @param	str		char array, needs not to be terminated
 * @param	count	number of characters from str
 *
 * @warning			Be aware of buffer overflows!
 */
void bt_uart_nputs(char *str, int count);

/**
 * @brief 		Print single character to UART.
 * @param	chr	Character to print to UART.
 */
void bt_uart_putc(char chr);

/**
 * @brief			Helper function that toggles the AT pin.
 * @param	state	\b true for AT mode, \b false for data mode
 */
void bt_switch_at_pin(bool state);

/**
 * @brief			Helper function that toggles the RESET pin.
 * @param	state	\b true for RESET, \b false for normal operation
 *
 * If RESET is set, bluetooth module sleeps with nearly no current consumption.
 */
void bt_switch_reset_pin(bool state);

/**
 * @brief 			Clears the RX buffer removing all remaining bytes.
 */
void bt_uart_clear_rx(void);

/**
 * @brief			Sends a command to BT module without raising RX interrupt.
 *
 * Disables the UART interrupt to receiving an answer from BT module blocking.
 * After answer is received interrupt is enabled.
 */
int bt_request(char request[], char response[], int response_length);


/**
 * @brief			Checks if module is in AT command mode.
 * @return			\b true for AT mode, \b false for transparent data mode
 */
bool bt_at_mode_active(void);

/**
 * @brief			Checks if module is paired and connected.
 * @return			\b true if connected, \b false otherwise
 *
 * This is useful to check if a variometer / device is connected.
 * If not, there should be no need to listen for radio packets.
 */
bool bt_is_connected(void);

#define bt_uart_puts_P bt_uart_puts

void bt_do_read(int len, char* data);



#endif /* BLUETOOTH_H_ */
