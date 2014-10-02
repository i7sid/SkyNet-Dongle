/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains the functionalities for CPU power management.
 */


#include "cpu.h"
#include "rtc.h"
#include "systick.h"
#include "../sysinit.h"
#include "../bluetooth/bluetooth.h"
#include "../radio/skynet_radio.h"
#include "../periph/adc.h"
#include "../periph/input.h"
#include "../periph/led.h"
#include "../periph/dcdc.h"


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
	dcdc_set_powersave(true);


	while (cpu_powered_down) {
		disable_systick();
		NVIC_EnableIRQ(INPUT_SWITCH_IRQn); 		// enable switch IRQ for wakeup
		NVIC_ClearPendingIRQ(INPUT_SWITCH_IRQn);

		rtc_prepare_powerdown();

		// disconnect and disable PLL0 (see Errata sheet, section 3.10)
		// (not sure if this could be done in one call to Chip_Clock_DisablePLL,
		//  so do it step by step)
		Chip_Clock_DisablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_CONNECT);
		while (Chip_Clock_IsMainPLLConnected()) {} // Wait to be disconnected
		Chip_Clock_DisablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);
		while (Chip_Clock_IsMainPLLEnabled()) {} // Wait to be disabled

		// And now go to bed!
		Chip_PMU_PowerDownState(LPC_PMU);

		SystemInit(); // restore IOCON and clocks (important for msDelay!)
		SystemCoreClockUpdate();
		cpu_set_speed(SPEED_120MHz);
		enable_systick(); // reenable SysTick functionality (updates clock)

		msDelay(2000);
		if (input_state() || charger_external_power()) {
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

	dcdc_set_powersave(false);
	adc_init();
	bt_init();

#ifdef SKYNET_RX_TEST
	radio_init();
#endif

	adc_start_buffered_measure();

	skynet_led_green(true);
	msDelay(1000);
	skynet_led_green(false);
}

