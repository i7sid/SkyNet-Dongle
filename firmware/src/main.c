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

#include "misc/misc.h"

#include "radio/radio.h"
#include "bluetooth/bluetooth.h"
#include "periph/led.h"
#include "periph/charger.h"
#include "periph/adc.h"
#include "cpu/cpu.h"
#include "misc/event_queue.h"

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000; // 12 MHz
const uint32_t RTCOscRateIn = 32768; // 32.768 kHz
#endif

extern uint8_t pwrLvlIdx;
extern uint8_t pwrLvl[];

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

    bool c = Chip_Clock_IsCrystalEnabled();
    DBG("%d\n", c);

    cpu_set_speed(SPEED_7M5Hz);

    DBG("Initialize RTC...\n");
    //rtc_init();

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

    DBG("Initialize Bluetooth module...\n");

    bt_init();	// initialize UART for bluetooth communication
    msDelay(500);
    //bt_shutdown();

    bt_enable_AT_mode();
    bt_uart_puts("AT+NAME?\r\n");
    msDelay(100);
    bt_uart_puts("AT+ROLE?\r\n");
    msDelay(100);
    bt_disable_AT_mode();

    // DEBUG test if AT mode successfully disabled
    bt_uart_puts("AT+ADDR\r\n");

    bt_enable_AT_mode();
	bt_uart_puts("AT+PSWD?\r\n");

	bt_disable_AT_mode();

    msDelay(500);



    DBG("Initialize radio module...\n");
    radio_init();

    msDelay(100); 	// wait a moment to ensure that all systems are up and ready




    DBG("Initialization complete.\n");
	vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, pRadioConfiguration->Radio_PacketLength);

	DBG("Radio RX started.\n");


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



    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter

    while(1) {
    	//ledOn(LED_RADIO);

#ifdef SKYNET_TX_TEST
    	skynet_led_red(true);
    	unsigned char data[] = "$SKT1,56812,JE00,0.836377,0.142920,852,40,0.0";
    	DBG("Sending packet: %s\n", data);
    	sendVariablePacket(data, sizeof(data));
    	DBG("Packet sent.\n");
    	//msDelay(500);
    	skynet_led_red(false);
    	//msDelay(2500);
#else
    	//Chip_PMU_SleepState(LPC_PMU);

    	event_types event = events_dequeue();
    	switch (event) {
			case EVENT_NONE:
				break;
			case EVENT_SWITCH_SINGLE:
				skynet_led_blue(true);
				msDelay(200);
				skynet_led_blue(false);
				bt_make_visible();
				break;
			case EVENT_SWITCH_DOUBLE:
				skynet_led_green(true);
				msDelay(200);
				skynet_led_green(false);
				break;
			case EVENT_SWITCH_POWER:
				cpu_powerdown();
				break;
			default:
				break;
		}

    	DBG("Charger State: STAT1: %i; STAT2: %i; EXTPWR: %i; TRUE: %i; V: %i\n", charger_status1(), charger_status2(), charger_external_power(), true, adc_measure());


    	//ledOff(LED_RADIO);
    	msDelay(500);
#endif

        i++ ;
    }
    return 0 ;
}
