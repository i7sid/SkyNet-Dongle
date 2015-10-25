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
#include "skynet_cdc.h"
#include "basestation/communikation/comprot.h"

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000; // 12 MHz
const uint32_t RTCOscRateIn = 32768; // 32.768 kHz
#endif

void usb_init(void);

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

	SPI_Init();

    DBG("Initialize Bluetooth module...\n");
    //bt_init();	// initialize bluetooth module communication comment in for real hw

    DBG("Initialize radio module...\n");
    //radio_init();
    //radio_pin_init();//comment in for real hw
    //radio_init();
    msDelay(100);  // wait a moment to ensure that all systems are up and ready


	skynet_led(true);
	msDelay(250);
	skynet_led(false);
	msDelay(200);
	skynet_led(true);
	msDelay(250);
	skynet_led(false);
	msDelay(200);
	skynet_led(true);
	msDelay(250);
	skynet_led(false);

    // usb init
    skynet_cdc_init();

    DBG("Initialization complete.\n");


    skynet_led(true);

    // usb test
    uint32_t rdCnt = 0;
    static uint8_t usb_cdc_rxBuff[256];
	while (1) {
		event_types event = events_dequeue();
		switch (event) {
			case EVENT_USB_DATA:
				rdCnt = skynet_cdc_read(&usb_cdc_rxBuff[0], 256);
				if (rdCnt) {
					skynet_led_blink_passive(5);
					skynet_cdc_write(&usb_cdc_rxBuff[0], rdCnt);
				}
				break;

			default:
				break;
		}

		// Sleep until next IRQ happens
		__WFI();
	}

	///// END OF USB DEBUG PROGRAM - lines below won't be reached ///////


    return 0 ;
}
