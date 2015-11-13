/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


///@brief Send a usb debug packet each second.
#define DEBUG_SEND_USB_TEST

///@brief Send a rf debug packet each second.
#define DEBUG_SEND_RF_TEST

///@brief This module is a basestation
//#define IS_BASESTATION


#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif


#include <cr_section_macros.h>
#include <string.h>
#include <stdlib.h>

#include "stopwatch.h"

#include "misc/misc.h"

#include "radio/skynet_radio.h"
#include "periph/input.h"
#include "periph/led.h"
#include "periph/charger.h"
#include "periph/adc.h"
#include "periph/dcdc.h"
#include "cpu/rtc.h"
#include "cpu/cpu.h"
#include "misc/event_queue.h"
#include "cmsis_175x_6x.h"
#include "skynet_cdc.h"
#include "basestation/skynet_basestation.h"

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000; // 12 MHz
const uint32_t RTCOscRateIn = 32768; // 32.768 kHz
#endif


void skynet_cdc_received_message(usb_message *msg);
void skynet_received_packet(skynet_packet *pkt);


void debug_send_usb(void) {
	events_enqueue(EVENT_DEBUG_1, NULL);
}

void debug_send_rf(void) {
	events_enqueue(EVENT_DEBUG_2, NULL);
}


int main(void) {


#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#endif
#endif
    DBG("Booting up...");

    SystemCoreClockUpdate();

    cpu_set_speed(SPEED_120MHz);

	StopWatch_Init();


#ifdef RTC_ENABLED
    DBG("Initialize RTC...\n");
    rtc_init();
#endif


    DBG("Clock: %d, %d\n", SystemCoreClock, Chip_Clock_GetCPUClockDiv());
    //Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_CONNECT);
    //Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);
    DBG("MainPLL: %d\n", Chip_Clock_IsMainPLLEnabled());

	// Initializes GPIO
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);
	events_init();
	skynet_led_init();
	charger_init();
	dcdc_init();


	// give visual feedback that program started
	skynet_led(true);
	msDelay(250);
	skynet_led(false);


    DBG("Initialize input...\n");
	input_init();
    DBG("Initialize ADC...\n");
	adc_init();
	adc_start_buffered_measure();

    DBG("Initialize radio module...\n");
    radio_init();
    msDelay(50);  // wait a moment to ensure that all systems are up and ready


    // usb init
    skynet_cdc_init();


#ifdef IS_BASESTATION
    // base station init
    skynetbase_init();
#endif



#ifdef DEBUG_SEND_USB_TEST
    // DEBUG: Send regularily usb packets
    register_delayed_event(1000, debug_send_usb);
#endif

#ifdef DEBUG_SEND_RF_TEST
    // DEBUG: Send regularily rf packets
    register_delayed_event(1000, debug_send_rf);
#endif

    DBG("Initialization complete.\n");


    skynet_led_blink_active(50);
    msDelay(50);
    skynet_led_blink_active(50);
    msDelay(50);
    skynet_led_blink_active(100);

	while (1) {

		queued_event event;
		event_types event_type = events_dequeue(&event);
		switch (event_type) {
			case EVENT_USB_RX_MESSAGE:
				skynet_cdc_received_message(event.data);
				break;
			case EVENT_RF_GOT_PACKET:
				skynet_received_packet(event.data);
				break;

			case EVENT_RADIO_RESTART:
				// TODO restart radio chip
				break;

			case EVENT_DEBUG_1:
			{
				// DEBUG: send usb packet
				char debugstr[] = "This is a debug string.";
				usb_message msg;
				memset(&msg, 0, sizeof(usb_message));
				msg.magic = USB_MAGIC_NUMBER;
				msg.type = USB_DEBUG;
				msg.seqno = 0;
				msg.payload_length = strlen(debugstr);
				msg.payload = debugstr;
				int cnt = skynet_cdc_write_message(&msg);
				DBG("n: %d\n", cnt);
				break;
			}
			case EVENT_DEBUG_2:
			{
				// DEBUG: send RF packet
				char* dbg_string = "Hello world! 0123456789 <=>?@";
				radio_send_variable_packet(dbg_string, strlen(dbg_string));
				break;
			}
			default: {
				break;
			}
		}

		// Sleep until next IRQ happens
		cpu_sleep();
	}

    return 0;
}

void skynet_received_packet(skynet_packet *pkt) {
	// send debug message
	usb_message msg;
	msg.seqno = 0;
	msg.type = USB_DEBUG;
	msg.payload_length = pkt->length;
	char buf[pkt->length];
	msg.payload = buf;
	memcpy(buf, pkt->data, pkt->length);

	skynet_cdc_write_message(&msg);
}


void skynet_cdc_received_message(usb_message *msg) {
	DBG("Received usb message of type %d.\n", msg->type);

	free(msg->payload);
	free(msg);
}


