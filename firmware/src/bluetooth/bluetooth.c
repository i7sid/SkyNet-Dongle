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

void bt_init() {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART3);

	// AT pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN);
	bt_switch_at_pin(false);

	// reset pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN);

	// reset module to be NOT in program mode
	bt_hardreset();

	// configure bluetooth UART
	bt_first_configuration();
	//bt_disable_AT_mode();

	// enable IRQ
	bt_uart_int_enable();

	// leave AT mode for immediate data transmission
	bt_disable_AT_mode();
}

void bt_uart_init(uint32_t baud) {
	// UART initialization, Interrupt initialization
	Chip_UART_Init(BLUETOOTH_UART);
	Chip_UART_SetBaud(BLUETOOTH_UART, baud); // should be: 115200
	Chip_UART_ConfigData(BLUETOOTH_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
	Chip_UART_SetupFIFOS(BLUETOOTH_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV1 | UART_FCR_RX_RS | UART_FCR_TX_RS ));
	Chip_UART_TXEnable(BLUETOOTH_UART);

	msDelay(500);
}

void bt_uart_deinit() {
	bt_uart_int_disable();
	Chip_UART_TXDisable(BLUETOOTH_UART);
	Chip_UART_DeInit(BLUETOOTH_UART);
}

INLINE void bt_uart_int_enable() {
	Chip_UART_IntEnable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
	NVIC_SetPriority(BLUETOOTH_UART_IRQn, 1); //TODO priority?
	NVIC_EnableIRQ(BLUETOOTH_UART_IRQn);
}

INLINE void bt_uart_int_disable() {
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_IntDisable(BLUETOOTH_UART, BLUETOOTH_UART_INT_MASK);
}

#define BLUETOOTH_AT_ANSWER 		("OK\r\n")
void bt_first_configuration() {
	char buf[24];
	int read = 0;
	memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

	bt_switch_at_pin(true);

	// first try to read VERSION at full speed
	bt_uart_init(115200);
	msDelay(500);		 // wait for UART fully up and ready
	bt_uart_puts("AT\r\n");
	msDelay(500);
	read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
	DBG("read (%d): %s\n", read, buf);
	bt_uart_clear_rx();


	if (strncmp(buf, BLUETOOTH_AT_ANSWER, sizeof(BLUETOOTH_AT_ANSWER) * sizeof(char)) != 0) {
		memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

		// speed not correct, try to fix it
		DBG("now fix baud rate\n");
		//bt_uart_deinit(); // shutdown UART

		/*
		bt_shutdown();	// shutdown bt module to reinit it in 38400 baud bode
		msDelay(50);
		bt_wakeup();	// wake up with AT pin high, so that module enters 38400 baud mode
		msDelay(50);
		*/
		bt_hardreset();

		bt_uart_init(38400); // reinit UART
		//msDelay(100);		 // wait for UART fully up and ready

		bt_uart_puts("AT\r\n");
		msDelay(50);
		read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
		DBG("read (AT) (%d): %s\n", read, buf);
		bt_uart_clear_rx();


		if (strncmp(buf, BLUETOOTH_AT_ANSWER, sizeof(BLUETOOTH_AT_ANSWER) * sizeof(char)) == 0) {
			memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

			bt_uart_puts("AT+UART=115200,0,0\r\n");
			msDelay(50);

			read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
			DBG("UART (AT+UART) (%d): %s\n", read, buf);

			bt_uart_clear_rx();

			//bt_uart_deinit(); // shutdown UART
			msDelay(100);
			bt_uart_init(115200); // reinit UART

			bt_shutdown();	// shutdown bt module to reinit it in 115200 baud bode
			bt_switch_at_pin(false);
			msDelay(100);
			bt_wakeup();	// wake up with AT pin high, so that module enters 115200 baud mode

			bt_switch_at_pin(true);
			msDelay(50);
		}
	}
	memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory

	//bt_change_visible(false);
}

void bt_deinit() {
	NVIC_DisableIRQ(BLUETOOTH_UART_IRQn);
	Chip_UART_DeInit(BLUETOOTH_UART);
}

void bt_reset() {
	bt_enable_AT_mode();
	bt_disable_AT_mode(); // disabling AT mode also resets the device
}

void bt_hardreset() {
	bt_shutdown();
	//msDelay(500);
	msDelay(500);
	bt_wakeup();
	//msDelay(1000);
	msDelay(500);
}
void bt_softreset() {
	bt_uart_puts("AT+RESET\r\n");
	msDelay(500);
}
void bt_enable_AT_mode() {
	msDelay(250);
	bt_switch_at_pin(true);
	msDelay(250);
}

void bt_disable_AT_mode() {
	msDelay(250);
	bt_switch_at_pin(false);
	//bt_uart_puts("AT+RESET\r\n");
	msDelay(50);
	bt_hardreset();
	msDelay(250);
	//bt_softreset();
	//msDelay(1000);
}

void bt_shutdown() {
	bt_switch_reset_pin(true);
}

void bt_wakeup() {
	// TODO: unsichtbar machen nötig?
	bt_switch_reset_pin(false);
}

void bt_make_visible() {
	bt_enable_AT_mode();
	bt_change_visible(true);
	bt_disable_AT_mode();
}

void bt_make_invisible() {
	bt_enable_AT_mode();
	bt_change_visible(false);
	bt_disable_AT_mode();
}

INLINE void bt_change_visible(bool visible) {
	char buf[6];

	switch (visible) {
		case true:
			bt_uart_puts("AT+IAC=928b33\r\n");
			//bt_uart_puts("AT+CLASS=1f00\r\n");
			break;
		default:
			bt_uart_puts("AT+IAC=928b30\r\n");
			//bt_uart_puts("AT+CLASS=3f00\r\n");
			break;
	}

	msDelay(100);
	//int read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));
	//DBG("set AT+IAC (%d): %s\n", read, buf);
	//bt_uart_clear_rx();
	//memset(buf, 0, sizeof(buf) * sizeof(char)); // empty buf stack memory
}

//TODO: send non-blocking ?
INLINE void bt_uart_puts(char *str) {
	DBG("BT TX: %s", str);
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
		//DBG("clear %d\n", read);
	}
}


void BLUETOOTH_UART_IRQ_HANDLER()
{
	DBG("UART3_IRQHandler\n");
	static char buf[BLUETOOTH_BUFFER_SIZE];
	int read = Chip_UART_Read(BLUETOOTH_UART, buf, sizeof(buf));

	if (read > 0) {
		buf[read] = '\0';

		//bt_do_read(read, buf);

		DBG("[BLUETOOTH_UART] read %d bytes: ", read);
		for (int i=0; i<read; ++i) {
			//DBG("%c (0x%x) |  ", buf[i], buf[i]);
			DBG("%c", buf[i], buf[i]);
		}
		DBG("\n");
	}
}

//TODO: durch sinnvollere Struktur ersetzen, z.B. RingBuffer o.ä.,
// damit nicht unnötig in InterruptHandler gearbeitet wird
//TODO: alten Ballast entfernen
#include "../radio/radio.h"
#define MYID 		4
void bt_do_read(int len, char* data) {
	int bat = 200;
	if (len > pRadioConfiguration->Radio_PacketLength + 7)
	{
		DBG("pktTooLong\n");
	}
	else if (len && strncmp((char *) data, "$SKT1,", 6) == 0)
	{
		uint8_t time = data[10] - '0';
		//for(uint8_t i=0; i<5; i++)
			//timeStr[i] = data[i+6];

		data[len] = 0;
		DBG("t\n");

		if ((time % 5) == MYID && bat > 175)
		{
			//ledOn(LED_BLUETOOTH);
			//DBG("%c", MYID);
			DBG(": tx\n");

			send_variable_packet(&data[6], len - 8);

			//$LOGT,PWR,ID\r\n
			bt_uart_puts_P("$LOGT,");
			data[11] = 0;				//replace ,
			data[16] = 0;				//shorten name to first 4 chars
			bt_uart_puts((char *) &data[6]);
			bt_uart_puts((char *) &data[12]);
			bt_uart_puts_P("\r\n");

		}
		else if(bat <= 175)
		{
			DBG("lowBat\n");
		}
		//enCnt = 0;
	}
	else if (len && strncmp((char *) data, "$INFO?", 6) == 0)
	{
		bt_uart_puts_P("$INFO,");
		bt_uart_putc(MYID + '0');
		bt_uart_putc(',');
		itoa(bat, (char *) data, 10);
		bt_uart_puts((char *) data);			// *0.02 = Vbat
		bt_uart_puts_P("\r\n");
	}
	else if(len != 0)
	{
		DBG("DBG: ");
		data[len] = 0;
		DBG("%s", data);
	}
}

INLINE void bt_switch_at_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_AT_PORT, BLUETOOTH_AT_PIN, state);
}

INLINE void bt_switch_reset_pin(bool state) {
	Chip_GPIO_SetPinState(LPC_GPIO, BLUETOOTH_RESET_PORT, BLUETOOTH_RESET_PIN, !state);
}
