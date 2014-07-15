/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include "cpu.h"
#include "../radio/skynet_radio.h"
#include "../bluetooth/bluetooth.h"

volatile bool cpu_powered_down = false;

void cpu_set_speed(cpu_speed speed) {
	switch (speed) {
		case SPEED_FULL:
		case SPEED_120MHz:
			Chip_Clock_SetCPUClockDiv(3);
			break;
		case SPEED_60MHz:
			Chip_Clock_SetCPUClockDiv(7);
			break;
		case SPEED_30MHz:
			Chip_Clock_SetCPUClockDiv(15);
			break;
		case SPEED_7M5Hz:
			Chip_Clock_SetCPUClockDiv(63);
			break;
		default:
			break;
	}

	DBG("CPU div: %d\n", Chip_Clock_GetCPUClockDiv());

}


void cpu_sleep() {
	//TODO
}


void cpu_powerdown() {
	cpu_powered_down = true;

	skynet_led_green(true);
	msDelay(100);
	skynet_led_green(false);
	msDelay(75);
	skynet_led_green(true);
	msDelay(100);
	skynet_led_green(false);

	bt_shutdown();
	radio_shutdown();

	//TODO: PLL trennen, sonstige?

	Chip_PMU_PowerDownState(PMU_MCU_DEEP_PWRDOWN);

	//TODO: Stromverbrauch prüfen
}


void cpu_wakeup() {
	skynet_led_green(true);
	msDelay(500);
	skynet_led_green(false);

	//TODO: Clocks wiederherstellen?
	radio_wakeup();
	bt_wakeup();
}
