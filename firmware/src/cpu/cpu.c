/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include "cpu.h"
#include "rtc.h"
#include "systick.h"
#include "../radio/skynet_radio.h"
#include "../bluetooth/bluetooth.h"
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


void cpu_sleep() {
	//TODO
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


	bt_shutdown();
	radio_shutdown();

	rtc_prepare_powerdown();


	//Chip_PMU_PowerDownState(LPC_PMU);


    //LPC_PMU->PCON |= PMU_PCON_PM1_FLAG | PMU_PCON_PM0_FLAG;
    LPC_PMU->PCON &= ~(PMU_PCON_PM1_FLAG | PMU_PCON_PM0_FLAG);
	SCB->SCR = SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPDEEP_Msk;
	//SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
	__WFI();

	//NVIC_SystemReset();






	//Chip_PMU_PowerDownState(LPC_PMU);

	//__WFI();
	/*
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_EnableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	Chip_PMU_DeepSleepState(LPC_PMU);
	*/


	skynet_led_green(true);
	msDelay(500);
	skynet_led_red(true);
	msDelay(500);
	skynet_led_blue(true);
	msDelay(1000);
	skynet_led_red(false);
	skynet_led_green(false);
	skynet_led_blue(false);


	//Chip_PMU_DeepPowerDownState(LPC_PMU);

	//TODO: Stromverbrauch prüfen
}

void cpu_repowerdown() {
	cpu_powered_down = true;

	Chip_RTC_Init(LPC_RTC);

    rtc_prepare_powerdown();

    LPC_PMU->PCON |= PMU_PCON_PM1_FLAG | PMU_PCON_PM0_FLAG;
	SCB->SCR = 4;
	__WFI();
}

void cpu_wakeup() {
	// TODO: nicht nötig bei Deep Power Down

	skynet_led_green(true);
	msDelay(500);
	skynet_led_green(false);

	//TODO: Clocks wiederherstellen?
	radio_wakeup();
	bt_wakeup(false);
}
