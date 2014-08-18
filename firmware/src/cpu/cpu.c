/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include "cpu.h"
#include "rtc.h"
#include "systick.h"
#include "../bluetooth/bluetooth.h"
#include "../radio/skynet_radio.h"
#include "../periph/adc.h"
#include "../periph/input.h"
#include "../periph/led.h"


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


INLINE void cpu_sleep() {
	Chip_PMU_SleepState(LPC_PMU);
}


void cpu_powerdown() {
	cpu_powered_down = true;

	skynet_led_red(true);
	skynet_led_green(true);
	skynet_led_blue(true);
	msDelay(500);
	skynet_led_green(false);
	msDelay(500);
	skynet_led_red(false);
	msDelay(500);
	skynet_led_blue(false);


	adc_deinit();
	bt_shutdown();
	radio_shutdown();

	while (cpu_powered_down) {
		NVIC_EnableIRQ(INPUT_SWITCH_IRQn); 		// enable switch IRQ for wakeup
		NVIC_ClearPendingIRQ(INPUT_SWITCH_IRQn);

		rtc_prepare_powerdown();

		// TODO disconnect and disable PLL0 (see Errata)

		Chip_PMU_DeepSleepState(LPC_PMU);
		//Chip_PMU_SleepState(LPC_PMU);

		SystemInit(); // restore IOCON and clocks (important for msDelay!)
		SystemCoreClockUpdate();
		cpu_set_speed(SPEED_30MHz);
		enable_systick(); // reenable SysTick functionality (updates clock)


		msDelay(2000);
		if (input_state()) {
			cpu_powered_down = false;
		}
	}


	skynet_led_green(true);
	skynet_led_red(true);
	skynet_led_blue(true);
	msDelayActive(2000);
	skynet_led_red(false);
	skynet_led_green(false);
	skynet_led_blue(false);

	adc_init();
	bt_init();
	radio_init();

	skynet_led_green(true);
	msDelay(1000);
	skynet_led_green(false);

	//TODO: Stromverbrauch prüfen
}

