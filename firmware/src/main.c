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
#include "cdc_vcom.h"
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

    // usb init
    usb_init();

    DBG("Initialization complete.\n");


    skynet_led_green(true);

    // usb test
    uint32_t prompt = 0, rdCnt = 0;
    static uint8_t usb_cdc_rxBuff[256];
	while (1) {
		// Check if host has connected and opened the VCOM port
		if ((vcom_connected() != 0) && (prompt == 0)) {
			vcom_write((uint8_t*)"Hello World!!\r\n", 15);
			prompt = 1;
		}
		// If VCOM port is opened echo whatever we receive back to host.
		if (prompt) {
			rdCnt = vcom_bread(&usb_cdc_rxBuff[0], 256);
			if (rdCnt) {
				vcom_write(&usb_cdc_rxBuff[0], rdCnt);
			}
		}
		// Sleep until next IRQ happens
		__WFI();
	}

	///// END OF USB DEBUG PROGRAM - lines below won't be reached ///////


    return 0 ;
}
