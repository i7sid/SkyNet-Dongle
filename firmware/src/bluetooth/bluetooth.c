/**
 * @file
 *
 * @date	25.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functions to communicate with the bluetooth module.
 */

#include <string.h>
#include <stdlib.h>

#include "bluetooth.h"
#include "../cpu/systick.h"
#include "../cpu/cpu.h"
#include "../misc/event_queue.h"

char bt_packet_rx_buf[BLUETOOTH_BUFFER_SIZE+1];
uint16_t bt_packet_rx_buf_pos = 0;
uint16_t bt_packet_rx_buf_written = 0;
static volatile bool visible = false;
static volatile bool highspeed = false;
static volatile bool waiting_for_answer = false;

void bt_init(void) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART3);

	// AT pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN);
	bt_switch_at_pin(false);

	// reset pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, true);

	// "connected" pin
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BLUETOOTH_CONNECTED_PORT, BLUETOOTH_CONNECTED_PIN);

	// "on/off" pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_ON_PORT, BLUETOOTH_ON_PIN);
	//Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_ON_PORT, BLUETOOTH_ON_PIN, false);


	bt_hardreset();		// start in normal operation mode


	// configure bluetooth UART
	bt_first_configuration();

	//go to high speed mode, now module is configured properly
	highspeed = true;
	bt_hardreset();		// start in normal operation mode


}

void bt_uart_init(uint32_t baud) {
	// UART initialization
	Chip_UART_Init(BLUETOOTH_UART);
	Chip_UART_SetBaud(BLUETOOTH_UART, baud);
	Chip_UART_ConfigData(BLUETOOTH_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
	Chip_UART_SetupFIFOS(BLUETOOTH_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0 | UART_FCR_RX_RS | UART_FCR_TX_RS ));
	Chip_UART_TXEnable(BLUETOOTH_UART);

	// enable IRQ
	bt_uart_int_enable();
}

void bt_uart_set_speed(uint32_t baud) {
	Chip_UART_SetBaud(BLUETOOTH_UART, baud);
}

void bt_uart_deinit(void) {
	bt_uart_int_disable();
	Chip_UART_TXDisable(BLUETOOTH_UART);
	Chip_UART_DeInit(BLUETOOTH_UART);
}

INLINE void bt_uart_int_enable(void) {
	Chip_UART_IntEnable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
	NVIC_SetPriority(BLUETOOTH_UART_IRQn, 3);
	NVIC_EnableIRQ(BLUETOOTH_UART_IRQn);
	NVIC_ClearPendingIRQ(BLUETOOTH_UART_IRQn);
}

INLINE void bt_uart_int_disable(void) {
	NVIC_ClearPendingIRQ(BLUETOOTH_UART_IRQn);
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_IntDisable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
}

void bt_first_configuration(void) {
	char buf[64];
	int read = 0;
	memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

	msDelayActive(100);
	bt_enable_AT_mode();

	msDelayActive(500);		// wait for UART fully up and ready
	read = bt_request("AT+NAME?\r\n", buf);
	buf[read] = '\0';
	DBG("AT+NAME?  (%d): %s\n", read, buf);
	DBG("should be (%d): %s\n", strlen(BLUETOOTH_NAME_ANSWER), BLUETOOTH_NAME_ANSWER);

	// if name is correctly set, assume that all settings are correct
	if (strncmp(buf, BLUETOOTH_NAME_ANSWER, strlen(BLUETOOTH_NAME_ANSWER) * sizeof(char)) != 0) {
		memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

		// settings not correct, fix it
		DBG("Bluetooth settings not correct yet, try to fix.\n");

		// set UART speed
		read = bt_request("AT+UART=115200,0,0\r\n", buf);
		buf[read] = '\0';
		DBG("UART (AT+UART) (%d): %s\n", read, buf);

		// set name
		read = bt_request(BLUETOOTH_SETNAME, buf);
		buf[read] = '\0';
		DBG("UART (AT+NAME) (%d): %s\n", read, buf);

		// set PIN
		read = bt_request(BLUETOOTH_SETPIN, buf);
		buf[read] = '\0';
		DBG("UART (AT+PSWD) (%d): %s\n", read, buf);
	}
}

void bt_deinit() {
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_DeInit(BLUETOOTH_UART);
}

void bt_reset() {
	bt_enable_AT_mode();
	bt_disable_AT_mode();
}

void bt_hardreset() {
	bt_shutdown();
	bt_wakeup();
}
void bt_softreset() {
	bt_enable_AT_mode();
	bt_uart_puts("AT+RESET\r\n");
	msDelay(1000);
}
void bt_enable_AT_mode() {
	// wait for previous transmission to be completed
	msDelayActive(BLUETOOTH_AT_PRESWITCH_DELAY);

	bt_switch_at_pin(true);

	// assure that module has enough time to recognize new state
	msDelayActive(BLUETOOTH_AT_POSTSWITCH_DELAY);
}

void bt_disable_AT_mode() {
	// wait for previous transmission to be completed
	msDelayActive(BLUETOOTH_AT_PRESWITCH_DELAY);

	bt_switch_at_pin(false);

	// assure that pin was toggled and module can recognize the changed state
	msDelayActive(BLUETOOTH_AT_POSTSWITCH_DELAY);
}

void bt_shutdown() {
	// wait for previous transmission to be completed
	msDelayActive(BLUETOOTH_BEFORE_SHUTDOWN_DELAY);

	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_ON_PORT, BLUETOOTH_ON_PIN, true);
	//Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, false);

	// wait for module to be fully shut down
	msDelayActive(BLUETOOTH_AFTER_SHUTDOWN_DELAY);
}

void bt_wakeup() {
	if (highspeed) {
		bt_disable_AT_mode();
		bt_uart_init(115200);	// configure UART for 115200 baud (full speed)
	}
	else {
		bt_enable_AT_mode();
		bt_uart_init(38400);	// configure UART for 38400 baud (fixed in AT mode)
	}

	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, true);
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_ON_PORT, BLUETOOTH_ON_PIN, false);	// power up!
	//Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, true);	// power up!

	// the module needs some time to resume work,
	// so wait  to guarantee functionality
	// (wait active to assure that pin can be set, don't sleep!)
	msDelayActive(BLUETOOTH_AFTER_WAKEUP_DELAY);

	if (!visible) {
		bt_enable_AT_mode();
		bt_change_visible(false);	// and now bring us to Ninja mode (invisble)

		/*
		// initialize -- otherwise no connection is possible if already paired before but invisible
		char tmpbuf[50];
		bt_request("AT+INIT\r\n", tmpbuf);
		DBG("AT+INIT: %s\n", tmpbuf);
*/
		bt_disable_AT_mode();
	}
}

INLINE void bt_make_visible() {
	visible = true;
	bt_hardreset();
}

INLINE void bt_make_invisible() {
	visible = false;
	bt_hardreset();
}

INLINE void bt_change_visible(bool visible) {
	char buf[11];
	int read = 0;

	switch (visible) {
		case true:
			//read = bt_request("AT+IAC=928b33\r\n", buf);
			read = bt_request("AT+IAC=9e8b33\r\n", buf);
			break;
		default:
			//read = bt_request("AT+IAC=928b30\r\n", buf);
			read = bt_request("AT+IAC=9e8b30\r\n", buf);
			break;
	}

	buf[read] = '\0';
	DBG("set AT+IAC (%d): %s\n", read, buf);

	/*
	msDelayActive(100);

	// take answer of AT+IAC command and ignore it
	// (it will be ERROR(E) even if it works...)
	int read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
	buf[read] = '\0';
	DBG("set AT+IAC (%d): %s\n", read, buf);
	bt_uart_clear_rx();
	*/
}

INLINE void bt_uart_puts(char *str) {
	//DBG("BT TX: %s", str);
	Chip_UART_SendBlocking(BLUETOOTH_UART, str, strlen(str));
}

INLINE void bt_uart_nputs(char *str, int count) {
	Chip_UART_SendBlocking(BLUETOOTH_UART, str, count);
}

INLINE void bt_uart_putc(char chr){
	//DBG("BT TX: %c\n", chr);
	Chip_UART_SendBlocking(BLUETOOTH_UART, &chr, 1);
}

void bt_uart_clear_rx() {
	int read = 1;
	static char buf[BLUETOOTH_BUFFER_SIZE];

	while (read > 0) {
		read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
		//DBG("clear %d %s\n", read, buf);
	}
}

int bt_request(char request[], char response[]) {
	// clear packet buffer
	waiting_for_answer = true;
	memset(bt_packet_rx_buf, 0, BLUETOOTH_BUFFER_SIZE);

	bt_uart_puts(request);

	int read = 0;
	while (read < 1) {
		if (strlen(bt_packet_rx_buf) > 0) {
			// received data, check if it's a full line
			char* n = strchr(bt_packet_rx_buf, '\n');
			if (n != NULL) {
				read = (size_t)n - (size_t)bt_packet_rx_buf + 1;
				bt_packet_rx_buf[read] = 0;
			}
		}
		else {
			cpu_sleep();
		}
	}


	waiting_for_answer = false;
	memcpy(response, bt_packet_rx_buf, read);
	DBG("ret: %s\n", response);
	return read;
}

void BLUETOOTH_UART_IRQ_HANDLER()
{
	static char buf[16+1];
	int read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(16));
	//buf[read] = '\0'; // not needed with this code
	//DBG("BT rx raw (%d): %s\n", read, buf);

	// reset length
	if (bt_packet_rx_buf_pos == 0) {
		bt_packet_rx_buf_written = 0;
	}

	// go through received bytes and look for newline character
	int i;
	for (i = 0; i < read; ++i) {
		bt_packet_rx_buf[bt_packet_rx_buf_pos + i] = buf[i];

		// newline character found?
		if (buf[i] == '\n') {
			if (!waiting_for_answer) {
				events_enqueue(EVENT_BT_GOT_PACKET);
			}
			bt_packet_rx_buf[bt_packet_rx_buf_pos + i + 1] = 0; // write trailing null byte for C
			bt_packet_rx_buf_written = bt_packet_rx_buf_pos; // save length
			bt_packet_rx_buf_pos = 0;
			return;
		}
	}
	bt_packet_rx_buf_pos += i;
}


INLINE void bt_switch_at_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN, state);
}

INLINE void bt_switch_reset_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, !state);
}

INLINE bool bt_at_mode_active(void) {
	return Chip_GPIO_GetPinState(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN);
}

bool bt_is_connected(void) {
	return Chip_GPIO_GetPinState(LPC_GPIO, BLUETOOTH_CONNECTED_PORT, BLUETOOTH_CONNECTED_PIN);
}
