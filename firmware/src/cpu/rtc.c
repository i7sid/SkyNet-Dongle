/**
 * @file
 *
 * @date	10.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */



#include "rtc.h"

RTC_TIME_T FullTime;


void rtc_init() {
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
}

void rtc_prepare_powerdown(void) {
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, DISABLE);
	Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR, ENABLE);
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	// TODO: verifizieren, dass die Überlaufbehandlung funktioniert/stimmt
	Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
	if (FullTime.time[RTC_TIMETYPE_SECOND] >= 57) {
		if (FullTime.time[RTC_TIMETYPE_MINUTE] >= 59) {
			if (FullTime.time[RTC_TIMETYPE_HOUR] >= 23) {
				FullTime.time[RTC_TIMETYPE_HOUR] = 0;
			}
			else {
				FullTime.time[RTC_TIMETYPE_HOUR] += 1;
			}
			FullTime.time[RTC_TIMETYPE_MINUTE] = 0;
		}
		else {
			FullTime.time[RTC_TIMETYPE_MINUTE] += 1;
		}
		FullTime.time[RTC_TIMETYPE_SECOND] = 0;
	}
	else {
		FullTime.time[RTC_TIMETYPE_SECOND] += 3;
	}

	Chip_RTC_SetFullAlarmTime(LPC_RTC, &FullTime);

	NVIC_SetPriority(RTC_IRQn, 1);
	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
}


void RTC_IRQHandler(void)
{
	//uint32_t sec;
	//DBG("RTC interrupt occured.\n");

	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		/// Clear pending interrupt
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);

		Chip_RTC_GetFullTime(LPC_RTC, &FullTime);

		DBG("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
				FullTime.time[RTC_TIMETYPE_HOUR],
				FullTime.time[RTC_TIMETYPE_MINUTE],
				FullTime.time[RTC_TIMETYPE_SECOND],
				FullTime.time[RTC_TIMETYPE_MONTH],
				FullTime.time[RTC_TIMETYPE_DAYOFMONTH],
				FullTime.time[RTC_TIMETYPE_YEAR]);
	}


	/*
	// display timestamp every 5 seconds in the background
	sec = Chip_RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
	if (!(sec % 5)) {
		fIntervalReached = true;	// set flag for background
	}
	*/

	// Check for alarm match
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		// Clear pending interrupt
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
		DBG("RTC ALARM!!!!\n");
		//fAlarmTimeMatched = true;	// set alarm handler flag
	}

}
