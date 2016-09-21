/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


///@brief Send a usb debug packet each second.
//#define DEBUG_SEND_USB_TEST

///@brief Send a rf debug packet each second.
//#define DEBUG_SEND_RF_TEST

#define RANGETEST_RX

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
#include <math.h>

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
#include "cpu/nv_storage.h"
#include "cpu/v_storage.h"
#include "skycom/rf_pkt_handler.h"

#include "mac/mac.h"
#include "mac/mac_extheader.h"

#include "skycom/dfx.h"
#include "skycom/LPC_SWU.h"

#define roundf(x)		{ ( (x - floorf(x)) < (ceilf(x) - x) ) ? floorf(x) : ceilf(x) }


__NOINIT(RAM2) volatile uint8_t goto_bootloader;
extern RTC_TIME_T FullTime;

extern uint8_t gpsout[128];


void skynet_cdc_received_message(usb_message *msg);


void debug_send_usb(void) {
	events_enqueue(EVENT_DEBUG_1, NULL);
	register_delayed_event(2000, debug_send_usb);
}

void debug_send_rf(void) {
	events_enqueue(EVENT_DEBUG_2, NULL);
	register_delayed_event(2000, debug_send_rf);
}

void ev_send_dfx(void) {
	events_enqueue(EVENT_DFX_SEND, NULL);
	register_delayed_event(100, ev_send_dfx);
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
    DBG("MainPLL online: %d\n", Chip_Clock_IsMainPLLEnabled());

	// Initializes GPIO
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);
	events_init();
	skynet_led_init();
	enable_systick();

	// give visual feedback that program started
	skynet_led(true);
	msDelay(250);
	skynet_led(false);


    DBG("Initialize ADC...\n");
	adc_init();
	//adc_start_buffered_measure();

    // init RNG (seed via ADC)
    unsigned int rand_seed = 0;
    for (uint8_t i = 0; i < 10; ++i) {
    	uint16_t data = 0;
    	adc_activate();

    	// Waiting for A/D conversion complete
    	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {}

    	// Read ADC value
    	Chip_ADC_ReadValue(LPC_ADC, ADC_CHANNEL, &data);
    	rand_seed += (data >> 1);

    	adc_deactivate();
    	msDelay(10);
    }
    DBG("%d\n", rand_seed);

    srand(rand_seed);

    //mac_init();
    skynet_nv_init();
    skynet_v_init();
    NV_DATA_T *config = skynet_nv_get();

    //DBG("Skip radio module.\n");
    DBG("Initialize radio module...\n");
    radio_init();
    vRadio_Change_PwrLvl(127);
    radio_start_rx();

    // init communication with adafruit feather
    //dfx_init();

    // usb init

    skynet_cdc_init();
    msDelay(700); // wait a moment to ensure that all systems are up and ready
    register_delayed_event(100, skynet_cdc_task);


	config->mac_addr[0] = 0x03;
	config->mac_addr[1] = 0x6B;
	config->mac_addr[2] = 0x84;
	config->mac_addr[3] = 0xA0;
	config->mac_addr[4] = 0x5C;
	config->mac_addr[5] = 0x26;


    DBG("Initialization complete.\n");


    skynet_led_blink_active(50);
    msDelay(50);
    skynet_led_blink_active(50);
    msDelay(50);
    skynet_led_blink_active(100);

//#define DEBUG
#ifndef DEBUG
    // initalize Watchdog
    Chip_WWDT_Init(LPC_WWDT);
    Chip_WWDT_SelClockSource(LPC_WWDT, WWDT_CLKSRC_WATCHDOG_PCLK);
    uint32_t wdtFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_WDT) / 4;
    Chip_WWDT_SetTimeOut(LPC_WWDT, 5 * wdtFreq);   // seconds * wdtFreq
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);
    Chip_WWDT_Start(LPC_WWDT);
#endif

#ifndef RANGETEST_RX
    register_delayed_event(2000, debug_send_rf);
#endif

    uint16_t seq = 0;
	while (1) {
#ifndef DEBUG
		Chip_WWDT_Feed(LPC_WWDT);
		//skynet_cdc_write_debug("Feed\n");
#endif

		// receive from usb
		//skynet_cdc_receive_data();


		queued_event event;
		event_types event_type = events_dequeue(&event);
		//if (event_type) DBG("dequeued: %d\n", event_type);
		switch (event_type) {
			case EVENT_USB_RX_MESSAGE:
				skynet_cdc_received_message(event.data);
				break;
			case EVENT_RF_GOT_PACKET:

				skynet_received_packet(event.data);

				break;
			case EVENT_DEBUG_2: {
			    //skynet_cdc_write_debug("Try to send...\n");
			    //skynet_led_blink_active(1000);

			    char radiopacket[33] = "";
				snprintf(radiopacket, sizeof(radiopacket), "%05d|789012345678901234567890123", seq);
				seq++;
				skynet_cdc_write_debug("Sending %s\n", radiopacket);

				radio_send_variable_packet((uint8_t*)radiopacket, sizeof(radiopacket));

				skynet_led_blink_passive(250);

				break;
			}
			case EVENT_DFX_SEND: {
				// do just nothin, was just for watchdog
				break;
			}
			default: {
				break;
			}
		}

		// Sleep until next IRQ happens
		cpu_sleep();
		//msDelay(1);
	}

    return 0;
}


void skynet_cdc_received_message(usb_message *msg) {
	switch(msg->type) {
		case USB_SKYNET_PACKET: {
//			mac_transmit_data((uint8_t*)msg->payload, msg->payload_length);
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
				case USB_CTRL_GET_MAC_ADDR:
				{
					NV_DATA_T *config = skynet_nv_get();
					skynet_cdc_write_debug("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
							config->mac_addr[0], config->mac_addr[1], config->mac_addr[2],
							config->mac_addr[3], config->mac_addr[4], config->mac_addr[5]);
					break;
				}
				default: {
					skynet_cdc_write_debug("Not implemented. Just small test utility.\n");
					break;
				}
			}
			break;
		}
		case USB_DEBUG:
			DBG("got usb debug packet, length: %d\n", msg->payload_length);
			for (uint16_t i = 0; i < msg->payload_length; ++i) {
				DBG("%d ", msg->payload[i]);
			}
			DBG("\n");
			break;
	}

	// Must be done! Memory was allocated dynamically.
	free(msg->payload);	free_count();
	free(msg);			free_count();
	skynet_led_blink_passive(5);
}


uint32_t sn_cnt_mallocs = 0;
uint32_t sn_cnt_frees = 0;
void malloc_count(void) { sn_cnt_mallocs++; }
void free_count(void) { sn_cnt_frees++; }
