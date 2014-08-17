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
#include "cpu/rtc.h"
#include "cpu/cpu.h"
#include "misc/event_queue.h"

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000; // 12 MHz
const uint32_t RTCOscRateIn = 32768; // 32.768 kHz
#endif

extern uint8_t pwrLvlIdx;
extern uint8_t pwrLvl[];
//extern char* packet_rx_buf;

/**
 * @brief	Sends a packet via radio.
 * @author	Jürgen Eckert
 */
void sendVariablePacket(uint8_t *pkt, uint8_t length)
{
	pwrLvlIdx = 0;
	DBG("txPKT\n");

	for (uint8_t pos = 0u; pos < pRadioConfiguration->Radio_PacketLength; pos++)
	{
		if (pos < length)
			customRadioPacket[pos+1] = pkt[pos];
		else
			customRadioPacket[pos+1] = 0;
	}

	vRadio_Change_PwrLvl(pwrLvl[pwrLvlIdx]);
	customRadioPacket[0] = pwrLvl[pwrLvlIdx];

	vRadio_StartTx_Variable_Packet(pRadioConfiguration->Radio_ChannelNumber, &customRadioPacket[0], pRadioConfiguration->Radio_PacketLength);
}


int main(void) {

#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();

	// TODO: replace with future debugging feature (other UART?)
	// DEBUGINIT();

#endif
#endif
    DBG("Booting up...");

    SystemCoreClockUpdate();

    cpu_set_speed(SPEED_30MHz);

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
	charger_set_mode(USB_CHARGE); // USB_CHARGE

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
    bt_init();	// initialize UART for bluetooth communication

    DBG("Initialize radio module...\n");
    radio_init();

    msDelay(100);  // wait a moment to ensure that all systems are up and ready

    /*

    // DEBUG: test current consumption
    bt_shutdown();
    bt_deinit();

    msDelayActive(1000);

    bt_init(); // implies wakeup; TODO: dokumentieren

    msDelayActive(1000);


    radio_shutdown();

    msDelayActive(1000);

    radio_init(); // implies wakeup; TODO: dokumentieren


    msDelayActive(1000);

*/


    DBG("Initialization complete.\n");


	//radio_config_for_clock_measurement();

    /*
	// measure core clock frequency

	si446x_get_property(0x0, 2, 0);
	DBG("GLOBAL_CLK_CFG: %x %x\n", Si446xCmd.GET_PROPERTY.DATA0, Si446xCmd.GET_PROPERTY.DATA1);

	Si446xCmd.GET_PROPERTY.DATA1 = 0b01100000;
	//si446x_set_property(0x0, 2, Si446xCmd.GET_PROPERTY.DATA0, Si446xCmd.GET_PROPERTY.DATA1);
    Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
    Pro2Cmd[1] = 0x0;
    Pro2Cmd[2] = 0x2;
    Pro2Cmd[3] = 0x0;
    Pro2Cmd[4] = Si446xCmd.GET_PROPERTY.DATA0;
    Pro2Cmd[5] = Si446xCmd.GET_PROPERTY.DATA1;

    radio_comm_SendCmd(6, Pro2Cmd);

    si446x_get_property(0x0, 2, 0);
	DBG("GLOBAL_CLK_CFG: %x %x\n", Si446xCmd.GET_PROPERTY.DATA0, Si446xCmd.GET_PROPERTY.DATA1);

     */


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


    volatile static int i = 0;
    while(1) {
    	//ledOn(LED_RADIO);

#ifdef SKYNET_TX_TEST
    	skynet_led_red(true);
    	unsigned char data[] = "$SKT1,56812,JE00,0.836377,0.142920,852,40,0.0";
    	DBG("Sending packet: %s\n", data);
    	sendVariablePacket(data, sizeof(data));
    	DBG("Packet sent.\n");

    	skynet_led_red(false);

    	msDelay(3000);
#else


    	event_types event = events_dequeue();
    	switch (event) {
			case EVENT_NONE:
				break;
			case EVENT_SWITCH_SINGLE:
				skynet_led_blue(true);
				bt_make_visible(); 		//TODO

				int timeout_cnt = 0;
				bool connected = false;
				while (!connected) {
					timeout_cnt++;
					if (timeout_cnt > 10) {
						bt_make_invisible();
						break;
					}
					bt_enable_AT_mode();

					msDelayActive(1000);		// give user time to pair

					char buf[25];
					int read = bt_request("AT+STATE?\r\n", buf, 24);
					DBG("STATE from BT module: %s\n", buf);
					buf[read] = '\0';

					if (!strncmp(buf, "+STATE:PAIRED", 13)) {
						DBG("Yeah, user paired!\n");
						connected = true;
					}

					//TODO: Erkennen, dass nicht mehr verbunden
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
				DBG("RF packet received: %s\n", packet_rx_buf);
				//TODO send via bluetooth
				break;
			default:
				break;
		}


    	if (i >= 100) {
    		i = 0;
    		skynet_led_green(true);
    		si446x_request_device_state();
			DBG("Charger State: STAT1: %i; STAT2: %i; EXTPWR: %i; TRUE: %i; V: %i; input: %i; GPIO0: %i; state: %i\n",
					charger_status1(), charger_status2(), charger_external_power(),
					true, adc_get_buffered_value(), input_state(), radio_get_gpio0(),
					Si446xCmd.REQUEST_DEVICE_STATE.CURR_STATE);
			msDelay(15);
			skynet_led_green(false);
    	}


    	//ledOff(LED_RADIO);
    	msDelay(10);
#endif

        i++ ;
    }
    return 0 ;
}