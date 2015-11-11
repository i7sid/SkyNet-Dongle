/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality to deal with CPU's Real Time Clock.
 */



#include "rtc.h"

extern RTC_TIME_T FullTime;


void rtc_init() {
#ifdef RTC_ENABLED
    Chip_RTC_Init(LPC_RTC);
	FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE]  = 0;
	FullTime.time[RTC_TIMETYPE_HOUR]    = 0;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 1;
	//FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 5;
	//FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 279;
	FullTime.time[RTC_TIMETYPE_MONTH]   = 1;
	FullTime.time[RTC_TIMETYPE_YEAR]    = 2014;
	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
#endif
}

void rtc_prepare_powerdown(void) {
#ifdef RTC_ENABLED
	// disable increment-interrupt
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, DISABLE);
#endif
}


void RTC_IRQHandler(void)
{
#ifdef RTC_ENABLED
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		/// Clear pending interrupt
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);

		Chip_RTC_GetFullTime(LPC_RTC, &FullTime);

		/*
		DBG("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
				FullTime.time[RTC_TIMETYPE_HOUR],
				FullTime.time[RTC_TIMETYPE_MINUTE],
				FullTime.time[RTC_TIMETYPE_SECOND],
				FullTime.time[RTC_TIMETYPE_MONTH],
				FullTime.time[RTC_TIMETYPE_DAYOFMONTH],
				FullTime.time[RTC_TIMETYPE_YEAR]);
		*/
	}

	// Check for alarm match
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		// Clear pending interrupt
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
		DBG("RTC ALARM!!!!\n");
		//fAlarmTimeMatched = true;	// set alarm handler flag
	}
#endif
}
