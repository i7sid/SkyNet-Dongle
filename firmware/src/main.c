/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


///@brief Send a usb debug packet each second.
//#define DEBUG_SEND_USB_TEST

///@brief Send a rf debug packet each second.
//#define DEBUG_SEND_RF_TEST


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
#include "basestation/skynet_basestation.h"
#include "cpu/nv_storage.h"
#include "cpu/v_storage.h"
#include "skycom/rf_pkt_handler.h"

#include "mac/mac.h"
#include "mac/mac_extheader.h"
#include "ip/udp.h"

#include "skycom/dfx.h"
#include "skycom/LPC_SWU.h"

#define roundf(x)		{ ( (x - floorf(x)) < (ceilf(x) - x) ) ? floorf(x) : ceilf(x) }


__NOINIT(RAM2) volatile uint8_t goto_bootloader;
extern RTC_TIME_T FullTime;

extern uint8_t gpsout[128];


void skynet_cdc_received_message(usb_message *msg);


void debug_send_usb(void) {
	events_enqueue(EVENT_DEBUG_1, NULL);
	register_delayed_event(1000, debug_send_usb);
}

void debug_send_rf(void) {
	events_enqueue(EVENT_DEBUG_2, NULL);
	register_delayed_event(10000, debug_send_rf);
}

void ev_send_dfx(void) {
	events_enqueue(EVENT_DFX_SEND, NULL);
	register_delayed_event(1000, ev_send_dfx);
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

    DBG("Skip radio module.\n");
    //DBG("Initialize radio module...\n");
    //radio_init();
    //vRadio_Change_PwrLvl(config->radio_pa_level);
    //radio_start_rx();

    // init communication with adafruit feather
    dfx_init();
    ev_send_dfx();

    // usb init
    /*
    skynet_cdc_init();
    msDelay(700); // wait a moment to ensure that all systems are up and ready
    register_delayed_event(500, skynet_cdc_task);
     */


#ifdef IS_BASESTATION


    // generate random mac address if none set
    if (config->mac_addr[0] == 0xFF ||
		config->mac_addr[1] == 0xFF ||
		config->mac_addr[2] == 0xFF ||
		config->mac_addr[3] == 0xFF ||
		config->mac_addr[4] == 0xFF ||
		config->mac_addr[5] == 0xFF ||
		config->mac_addr[0] == 0x0 ||
		config->mac_addr[1] == 0x0 ||
		config->mac_addr[2] == 0x0 ||
		config->mac_addr[3] == 0x0 ||
		config->mac_addr[4] == 0x0 ||
		config->mac_addr[5] == 0x0
		) {

    	config->mac_addr[0] = 0x03;
    	config->mac_addr[1] = 0x6B;
    	config->mac_addr[2] = 0x84;
    	config->mac_addr[3] = 0xA0;

    	uint8_t rand1 = (rand() & 0xFF);
    	while (rand1 == 0x0 || rand1 == 0xFF) rand1 = (rand() & 0xFF);
    	config->mac_addr[4] = rand1;

    	rand1 = (rand() & 0xFF);
    	while (rand1 == 0x0 || rand1 == 0xFF) rand1 = (rand() & 0xFF);
    	config->mac_addr[5] = rand1;

    	skynet_nv_write(config);
    }

    // base station init
    skynetbase_init();

    // load some default data
    if ((*((char*)&(config->compass_calibration.declination))) == 0xFF) {
    	config->compass_calibration.declination = 2.366667f; // Erlangen: 2° 22'
    }
    if ((*((char*)&(config->compass_calibration.offset_x))) == 0xFF) {
    	config->compass_calibration.offset_x = 0;
    }
    if ((*((char*)&(config->compass_calibration.offset_y))) == 0xFF) {
    	config->compass_calibration.offset_y = 0;
    }
    if ((*((char*)&(config->compass_calibration.offset_z))) == 0xFF) {
    	config->compass_calibration.offset_z = 0;
    }
    if ((*((char*)&(config->compass_calibration.factor_z))) == 0xFF) {
    	config->compass_calibration.factor_z = 1;
    }

    // load calibration data
    skynetbase_compass_calibration_set(&(config->compass_calibration));

    v_configuration.time_wind_wait = 500;

    // send regularily data events
    register_delayed_event(v_configuration.time_wind_wait, generate_event_wind_base);
	register_delayed_event(v_configuration.time_pos_wait, generate_event_pos_base);

	// start first GPS position query
	skynetbase_gps_query();
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

#ifndef DEBUG
    // initalize Watchdog
    Chip_WWDT_Init(LPC_WWDT);
    Chip_WWDT_SelClockSource(LPC_WWDT, WWDT_CLKSRC_WATCHDOG_PCLK);
    uint32_t wdtFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_WDT) / 4;
    Chip_WWDT_SetTimeOut(LPC_WWDT, 3 * wdtFreq);   // seconds * wdtFreq
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);
    Chip_WWDT_Start(LPC_WWDT);
#endif

    //register_delayed_event(10000, debug_send_rf);

	while (1) {
#ifndef DEBUG
		Chip_WWDT_Feed(LPC_WWDT);
#endif

		// receive from usb
		//skynet_cdc_receive_data();


#ifdef IS_BASESTATION
		// receive GPS uart data
    	skynetbase_gps_receive_data();
#endif

		queued_event event;
		event_types event_type = events_dequeue(&event);
		//if (event_type) DBG("dequeued: %d\n", event_type);
		switch (event_type) {
			case EVENT_USB_RX_MESSAGE:
				skynet_cdc_received_message(event.data);
				break;
			case EVENT_RF_GOT_PACKET:
				//skynet_received_packet(event.data);
				break;

			case EVENT_BASE_QUERY_POS:
			{
				// start next query
				skynetbase_gps_query();
				break;
			}
			case EVENT_BASE_SEND_WIND:
			{
				break;
			}
			case EVENT_GPS_DATA_AVAILABLE:
			{
				//DBG("GPS data received.\n");
				break;
			}
			case EVENT_DFX_SEND:
			{
				gps_pubx_data* gps = skynetbase_gps_get_data();
				Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
				float compass = skynetbase_compass_read();
				float windspeed = skynetbase_windspeed_get();
				uint16_t wind_dir_raw = skynetbase_windvane_measure();
				uint16_t wind_dir = wind_dir_raw;

				// wind dir: compensate orientation of station via compass
				wind_dir += compass;
				wind_dir = wind_dir % 360;

				// start next query
				skynetbase_gps_query();


				if (!(gps->status) || !(gps->lat_dir) || !(gps->lon_dir) || !(gps->lat) || !(gps->lon)) {
					// TODO send notification to network?
					break; // no gps data available yet
				}


				uint8_t pos = 0;
				uint8_t buf[128];

				pos += snprintf((char*)buf, sizeof(buf) - pos,
						"%02d%02d%02d|%c:%f:%c:%f|%f|%d|%f|",
						FullTime.time[RTC_TIMETYPE_HOUR],
						FullTime.time[RTC_TIMETYPE_MINUTE],
						FullTime.time[RTC_TIMETYPE_SECOND],
						gps->lat_dir, gps->lat, gps->lon_dir, gps->lon, compass,
						wind_dir, windspeed);


				uint8_t chksum = dfx_checksum_calc(buf, pos);
				pos += snprintf((char*)buf + pos, sizeof(buf) - pos,
						"%.2x", chksum);

				pos++; // trailing null byte of string (written by snprintf, missing in return value "pos")

				for (uint16_t i = 0; i < pos; ++i) {
					swu_tx_chr(buf[i]);
				}

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
#if DEBUG
/*
	DBG("Received usb message of type %d.\n", msg->type);
	for (int i = msg->payload_length - 8; i < msg->payload_length; ++i) {
		DBG("0x%x ", (msg->payload[i] & 0xFF));
	}
	DBG("\n");
*/
#endif

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
				case USB_CTRL_CALIB_COMPASS:
					// TODO
					break;
				case USB_CTRL_SET_MAC_ADDR:
				{
					NV_DATA_T *config = skynet_nv_get();
					config->mac_addr[0] = msg->payload[1];
					config->mac_addr[1] = msg->payload[2];
					config->mac_addr[2] = msg->payload[3];
					config->mac_addr[3] = msg->payload[4];
					config->mac_addr[4] = msg->payload[5];
					config->mac_addr[5] = msg->payload[6];
					bool r = skynet_nv_write(config);
					if (r) {
						skynet_cdc_write_debug("OK\n");
					}
					else {
						skynet_cdc_write_debug("Error\n");
					}
					break;
				}
				case USB_CTRL_GET_MAC_ADDR:
				{
					NV_DATA_T *config = skynet_nv_get();
					skynet_cdc_write_debug("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
							config->mac_addr[0], config->mac_addr[1], config->mac_addr[2],
							config->mac_addr[3], config->mac_addr[4], config->mac_addr[5]);
					break;
				}
				case USB_CTRL_SET_IP_ADDR:
				{
					NV_DATA_T *config = skynet_nv_get();
					config->ipv4_addr[0] = msg->payload[1];
					config->ipv4_addr[1] = msg->payload[2];
					config->ipv4_addr[2] = msg->payload[3];
					config->ipv4_addr[3] = msg->payload[4];
					bool r = skynet_nv_write(config);
					if (r) {
						skynet_cdc_write_debug("OK\n");
					}
					else {
						skynet_cdc_write_debug("Error\n");
					}
					break;
				}
				case USB_CTRL_GET_IP_ADDR:
				{
					NV_DATA_T *config = skynet_nv_get();
					skynet_cdc_write_debug("IP: %d.%d.%d.%d\n",
							config->ipv4_addr[0], config->ipv4_addr[1],
							config->ipv4_addr[2], config->ipv4_addr[3]);
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
