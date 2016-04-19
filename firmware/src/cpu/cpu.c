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
#include "../radio/skynet_radio.h"
#include "../periph/adc.h"
#include "../periph/led.h"


volatile bool cpu_powered_down = false;
extern volatile uint8_t goto_bootloader;

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

void cpu_reset(void) {
	goto_bootloader = 0x0;
	NVIC_SystemReset();
}

void cpu_enter_iap_mode(void) {
	goto_bootloader = 0xAA;
	NVIC_SystemReset();
}

