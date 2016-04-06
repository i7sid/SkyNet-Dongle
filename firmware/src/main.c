/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


///@brief Send a usb debug packet each second.
#define DEBUG_SEND_USB_TEST

///@brief Send a rf debug packet each second.
//#define DEBUG_SEND_RF_TEST

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
#include "periph/led.h"
#include "periph/adc.h"
#include "cpu/rtc.h"
#include "cpu/cpu.h"
#include "misc/event_queue.h"
#include "cmsis_175x_6x.h"
#include "skynet_cdc.h"
#include "basestation/skynet_basestation.h"

#include "mac/mac.h"

__NOINIT(RAM2) volatile uint8_t goto_bootloader;
extern RTC_TIME_T FullTime;


void skynet_cdc_received_message(usb_message *msg);
void skynet_received_packet(skynet_packet *pkt);


void debug_send_usb(void) {
	events_enqueue(EVENT_DEBUG_1, NULL);
	register_delayed_event(1000, debug_send_usb);
}

void debug_send_rf(void) {
	events_enqueue(EVENT_DEBUG_2, NULL);
	register_delayed_event(1000, debug_send_rf);
}


int main(void) {
	goto_bootloader = 0;


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
	//charger_init();
	//dcdc_init();
	enable_systick();

	// give visual feedback that program started
	skynet_led(true);
	msDelay(250);
	skynet_led(false);



    DBG("Initialize ADC...\n");
	adc_init();
	adc_start_buffered_measure();

    DBG("Initialize radio module...\n");
    radio_init();

    // init RNG
    Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
    srand(FullTime.time[RTC_TIMETYPE_SECOND] * FullTime.time[RTC_TIMETYPE_MINUTE] *
    		FullTime.time[RTC_TIMETYPE_HOUR] * FullTime.time[RTC_TIMETYPE_DAYOFYEAR]);
    // TODO better seed

    // usb init
    skynet_cdc_init();
    msDelay(700); // wait a moment to ensure that all systems are up and ready




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
		skynet_cdc_receive_data();

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
				skynet_cdc_write_debug("%s\n", debugstr);

				skynet_led_blink_active(50);
				msDelayActive(50);
				skynet_led_blink_active(50);
				break;
			}
			case EVENT_DEBUG_2:
			{
				// DEBUG: send RF packet
				uint8_t p[] = "Test-Payload";
				mac_frame_data frame;
				mac_frame_data_init(&frame);
				frame.payload = p;
				frame.payload_size = strlen((char*)p) + 1;

				mac_transmit_packet(&frame);
				/*
				uint8_t buffer[mac_frame_data_get_size(&frame)];
				mac_frame_data_pack(&frame, buffer);
				mac_frame_calc_crc(buffer, sizeof(buffer));

				mac_transmit_packet(buffer, sizeof(buffer));
				*/


				/*
				char* dbg_string = "Hello world! 0123456789 <=>?@";
				radio_send_variable_packet((uint8_t *)dbg_string, (uint16_t)strlen(dbg_string));
				*/

				skynet_led_blink_passive(100);
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
	DBG("pkt recv (length: %d)\n", pkt->length);
	// send debug message
	usb_message msg;
	msg.seqno = 0; 							// chose automatically next one
	msg.type = USB_SKYNET_PACKET;
	msg.payload_length = pkt->length;
	char buf[pkt->length];
	msg.payload = buf;
	memcpy(buf, pkt->data, pkt->length);

	skynet_cdc_write_message(&msg);

	free(pkt->data);
	free(pkt);
}


void skynet_cdc_received_message(usb_message *msg) {
	DBG("Received usb message of type %d.\n", msg->type);

	switch(msg->type) {
		case USB_SKYNET_PACKET: {
			// TODO richtig auspacken und senden
			mac_transmit_data((uint8_t*)msg->payload, msg->payload_length);
//			mac_transmit_packet((uint8_t*)msg->payload, msg->payload_length);
			break;
		}
		case USB_CONTROL: {
			switch((usb_ctrl_msg_type)(msg->payload[0])) {
				case USB_CTRL_RESET:
					cpu_reset();
					break;
				case USB_CTRL_BOOTLOADER:
					cpu_enter_iap_mode();
					break;
			}
			break;
		}
	}

	// Must be done! Memory was allocated dynamically.
	free(msg->payload);
	free(msg);
}
