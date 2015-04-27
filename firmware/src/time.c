/*
 * time.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "time.h"

int timeAdd(RTC_TIME_T* time){
	int ret = time->time[RTC_TIMETYPE_SECOND];
	ret += (time->time[RTC_TIMETYPE_MINUTE])*60;
	ret += (time->time[RTC_TIMETYPE_HOUR])*3600;
	return ret;
}
