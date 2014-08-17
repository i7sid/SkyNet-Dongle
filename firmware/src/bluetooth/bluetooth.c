/**
 * @file
 *
 * @date	25.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <string.h>
#include <stdlib.h>

#include "bluetooth.h"
#include "../cpu/systick.h"

static volatile bool visible = false;

void bt_init() {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART3);

	// AT pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN);
	bt_switch_at_pin(false);

	// reset pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN);

	bt_hardreset();		// start in normal operation mode

	// configure bluetooth UART
	bt_first_configuration();

	// enable IRQ
	//bt_uart_int_enable();
}

void bt_uart_init(uint32_t baud) {
	// UART initialization
	Chip_UART_Init(BLUETOOTH_UART);
	Chip_UART_SetBaud(BLUETOOTH_UART, baud);
	Chip_UART_ConfigData(BLUETOOTH_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
	Chip_UART_SetupFIFOS(BLUETOOTH_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV1 | UART_FCR_RX_RS | UART_FCR_TX_RS ));
	Chip_UART_TXEnable(BLUETOOTH_UART);
}

void bt_uart_set_speed(uint32_t baud) {
	Chip_UART_SetBaud(BLUETOOTH_UART, baud);
}

void bt_uart_deinit() {
	bt_uart_int_disable();
	Chip_UART_TXDisable(BLUETOOTH_UART);
	Chip_UART_DeInit(BLUETOOTH_UART);
}

INLINE void bt_uart_int_enable() {
	Chip_UART_IntEnable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
	NVIC_SetPriority(BLUETOOTH_UART_IRQn, 3); //TODO priority?
	NVIC_EnableIRQ(BLUETOOTH_UART_IRQn);
	NVIC_ClearPendingIRQ(BLUETOOTH_UART_IRQn);
}

INLINE void bt_uart_int_disable() {
	NVIC_ClearPendingIRQ(BLUETOOTH_UART_IRQn);
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_IntDisable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
}

#define BLUETOOTH_AT_ANSWER 		("OK\r\n")
void bt_first_configuration() {
	char buf[24];
	int read = 0;
	memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

	msDelayActive(100);
	bt_enable_AT_mode();

	// first try to read AT => OK at full speed
	//bt_uart_init(115200);	// assure that we are use full speed
	msDelayActive(500);		// wait for UART fully up and ready
	read = bt_request("AT\r\n", buf, 23);
	buf[read] = '\0';
	DBG("AT (%d): %s\n", read, buf);

	if (strncmp(buf, BLUETOOTH_AT_ANSWER, sizeof(BLUETOOTH_AT_ANSWER) * sizeof(char)) != 0) {
		memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

		// speed not correct, try to fix it
		DBG("now fix baud rate\n");

		bt_shutdown();
		bt_wakeup();		// restart in AT mode

		read = bt_request("AT\r\n", buf, 23);
		buf[read] = '\0';
		DBG("read (AT) (%d): %s\n", read, buf);
		bt_uart_clear_rx();


		if (strncmp(buf, BLUETOOTH_AT_ANSWER, sizeof(BLUETOOTH_AT_ANSWER) * sizeof(char)) == 0) {
			memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

			// set UART speed
			read = bt_request("AT+UART=115200,0,0\r\n", buf, 23);
			buf[read] = '\0';
			DBG("UART (AT+UART) (%d): %s\n", read, buf);

			// set name
			read = bt_request("AT+NAME=SKYNET\r\n", buf, 23);
			buf[read] = '\0';
			DBG("UART (AT+NAME) (%d): %s\n", read, buf);
		}
	}
}

void bt_deinit() {
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_DeInit(BLUETOOTH_UART);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_UART3);
}

void bt_reset() {
	bt_enable_AT_mode();
	bt_disable_AT_mode(); // disabling AT mode also resets the device
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
	msDelayActive(100);

	bt_switch_at_pin(true);

	// assure that module has enough time to recognize new state
	msDelayActive(250);
}

void bt_disable_AT_mode() {
	// wait for previous transmission to be completed
	msDelayActive(100);

	bt_switch_at_pin(false);

	// assure that pin was toggled and module can recognize the changed state
	msDelayActive(250);
}

void bt_shutdown() {
	// wait for previous transmission to be completed
	msDelayActive(100);

	bt_switch_reset_pin(true);
	// wait for module to be fully shut down
	msDelayActive(1500);
}

void bt_wakeup() {
	if (visible) {
		bt_disable_AT_mode();
		bt_uart_init(115200);	// configure UART for 115200 baud (full speed)
	}
	else {
		bt_enable_AT_mode();	// start in full AT mode
		bt_uart_init(38400);	// configure UART for 38400 baud (fixed in AT mode)
	}

	bt_switch_reset_pin(false);	// power up!


	// the module needs some time to resume work,
	// so wait  to guarantee functionality
	// (wait active to assure that pin can be set, don't sleep!)
	msDelayActive(1500);

	if (!visible) {
		bt_change_visible(false);	// and now bring us to Ninja mode (invisble)
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
			read = bt_request("AT+IAC=928b33\r\n", buf, 10);
			break;
		default:
			read = bt_request("AT+IAC=928b30\r\n", buf, 10);
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

//TODO: send non-blocking ?
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

int bt_request(char request[], char response[], int response_length) {
	bt_uart_int_disable();
	msDelayActive(50);

	bt_uart_puts(request);
	msDelayActive(500);
	int read = Chip_UART_Read(BLUETOOTH_UART, response, response_length);
	msDelayActive(100);
	bt_uart_clear_rx();

	msDelayActive(50);
	bt_uart_int_enable();
	return read;
}

void BLUETOOTH_UART_IRQ_HANDLER()
{
	static char buf[BLUETOOTH_BUFFER_SIZE];
	int read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));

	if (read > 0) {
		buf[read] = '\0';

		DBG("[BLUETOOTH_UART IRQ] read %d bytes: ", read);
		for (int i=0; i<read; ++i) {
			DBG("%c", buf[i], buf[i]);
		}
		DBG("\n");
	}
}


INLINE void bt_switch_at_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN, state);
}

INLINE void bt_switch_reset_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, !state);
}