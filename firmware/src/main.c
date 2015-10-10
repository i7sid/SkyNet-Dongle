/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


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
#include "bluetooth/bluetooth.h"
#include "periph/input.h"
#include "periph/led.h"
#include "periph/charger.h"
#include "periph/adc.h"
#include "periph/dcdc.h"
#include "cpu/rtc.h"
#include "cpu/cpu.h"
#include "misc/event_queue.h"
#include "basestation/skybase.h"
#include "cmsis_175x_6x.h"
#include "basestation/communikation/comprot.h"

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000; // 12 MHz
const uint32_t RTCOscRateIn = 32768; // 32.768 kHz
#endif


int main(void) {


#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#endif
#endif
    DBG("Booting up...");

    SystemCoreClockUpdate();

    cpu_set_speed(SPEED_60MHz);

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
	skynet_led_init();
	skynet_led_blue(false);
	skynet_led_green(false);
	skynet_led_red(false);
	charger_init();
	dcdc_init();


	// give visual feedback that program started
	skynet_led_red(true);
	skynet_led_green(true);
	skynet_led_blue(true);
	msDelay(250);
	skynet_led_red(false);
	skynet_led_green(false);
	skynet_led_blue(false);


    DBG("Initialize input...\n");
	input_init();
    DBG("Initialize ADC...\n");
	adc_init();
	adc_start_buffered_measure();

	SPI_Init();

    DBG("Initialize Bluetooth module...\n");
    //bt_init();	// initialize bluetooth module communication comment in for real hw

    DBG("Initialize radio module...\n");
    //radio_init();
    //radio_pin_init();//comment in for real hw
    //radio_init();
    msDelay(100);  // wait a moment to ensure that all systems are up and ready

    DBG("Initialize Basestation Module...\n");
    baseinit();

    DBG("Initialization complete.\n");


	skynet_led_green(true);
	msDelay(250);
	skynet_led_green(false);
	msDelay(200);
	skynet_led_green(true);
	msDelay(250);
	skynet_led_green(false);
	msDelay(200);
	skynet_led_green(true);
	msDelay(250);
	skynet_led_green(false);


	//uint32_t last_bt_check = StopWatch_Start();
	//bool bt_connected = false;



#ifdef SKYNET_TX_TEST
    DBG("Initialize radio module...\n");
    radio_init();

	unsigned char data[8192];
	data[8191] = 0;

	for (int i=0; i < 8191; i += 2) {
		//data[i] = (i % 10) + 48;
		data[i] = (i/60) + 48;
		data[i+1] = (i % 60) + 65;
		//DBG("write data: %c at %d\n", data[i], i);
	}


#endif
#ifdef SKYNET_RX_TEST
    DBG("Initialize radio module...\n");
    radio_init();
#endif

    volatile static int i = 0;
    while(1) {

    	//if((i % 10) == 0){
    	//	updateBaseData();
    	//}

#ifdef SKYNET_TX_TEST
    	msDelay(1000);

    	skynet_led_red(true);

    	DBG("Sending packet: %s\n", data);
    	//sendVariablePacket(data, 76);
    	//sendLongPacket(data, 76);
    	vRadio_Change_PwrLvl(127);
    	radio_send_variable_packet(SKYNET_RADIO_TESTPATTERN, SKYNET_RADIO_TESTLENGTH);
    	//radio_send_variable_packet(data, 4096);
    	DBG("Packet sent.\n");

    	skynet_led_red(false);

    	msDelay(2000);
#else

    	event_types event = events_dequeue();
    	switch (event) {
			case EVENT_NONE:
				break;
			case EVENT_SWITCH_SINGLE:
				skynet_led_blue(true);
				bt_make_visible();

				int timeout_cnt = 0;
				bool connected = false;
				while (!connected) {
					timeout_cnt++;
					if (timeout_cnt > 15) {
						bt_make_invisible();
						break;
					}
					msDelay(1000);	// give user time to pair
					connected = bt_is_connected();
				}
				skynet_led_blue(false);
				break;
			case EVENT_SWITCH_DOUBLE:
				skynet_led_green(true);
				msDelay(200);
				skynet_led_green(false);
				break;
			case EVENT_SWITCH_POWER:
				cpu_powerdown();
				break;
			case EVENT_RF_GOT_PACKET:
				DBG("RF packet received: %s\n", rf_packet_rx_buf);
				skynet_led_blink_blue_passive(50);
				skynet_led_blink_green_passive(50);
				/*
				if (!bt_at_mode_active() && bt_is_connected()) {
					bt_uart_puts(rf_packet_rx_buf);
				}
				*/
				get_config_message(rf_packet_rx_buf);

				break;
			case EVENT_BT_GOT_PACKET:
				DBG("BT packet received (size: %d): %s\n", bt_packet_rx_buf_written, bt_packet_rx_buf);

				if (bt_is_connected()) {
					radio_send_variable_packet(bt_packet_rx_buf, bt_packet_rx_buf_written);
					bt_uart_nputs(bt_packet_rx_buf, bt_packet_rx_buf_written);
					skynet_led_blink_blue_passive(100);
				}
				break;
			case EVENT_LOW_BATTERY:
				skynet_led_blink_red_passive(100);
				break;
			case EVENT_CRITICAL_BATTERY:
				cpu_powerdown();
				break;
			case EVENT_RADIO_RESTART:
				radio_shutdown();
				msDelayActive(50);
				msDelay(100);
				radio_init();
				break;
			default:
				break;
		}



    	// check if bluetooth device connected fixme not needed as basestation
    	// if not - no radio required
    	/*
    	if (StopWatch_TicksToMs(StopWatch_Elapsed(last_bt_check)) > 1000) {
    		bool c = bt_is_connected();
    		if (!bt_connected && c) {
    			DBG("Initialize radio module...\n");
    			radio_init();
    		}
    		else if (bt_connected && !c) {
    			DBG("Shutdown radio module...\n");
    			radio_shutdown();

    			// and switch back to hidden low speed UART mode
    			bt_make_invisible();
    		}
    		bt_connected = c;

    		// restart stopwatch
    		last_bt_check = StopWatch_Start();
    	}
    	*/


#ifdef DEBUG
    	// DEBUG Outputs
    	//if (i >= 100) { //comment in for real hw
    	if (false) { //comment out for real hw
    		i = 0;
    		DBG("Enter debug block\n");
    		skynet_led_green(true);
    		si446x_request_device_state();
			DBG("Charger State: STAT1: %i; STAT2: %i; EXTPWR: %i; TRUE: %i; V: %i; input: %i; GPIO0: %i; state: %i; bt_connected: %i\n",
					charger_status1(), charger_status2(), charger_external_power(),
					true, adc_get_buffered_value(), input_state(), radio_get_gpio0(),
					Si446xCmd.REQUEST_DEVICE_STATE.CURR_STATE, bt_is_connected());
			msDelay(15);
			skynet_led_green(false);
    	}

    	msDelay(10);
#else
    	if (i >= 1000000) {
    		i = 0;
    		skynet_led_blink_green_passive(20);
    	}
#endif
#endif

        i++ ;
    }
    return 0 ;
}
