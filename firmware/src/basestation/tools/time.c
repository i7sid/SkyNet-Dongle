/*
 * time.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "time.h"

long timeAdd(RTC_TIME_T* time, int ms){
	int ret = time->time[RTC_TIMETYPE_SECOND];
	ret += (time->time[RTC_TIMETYPE_MINUTE])*60;
	ret += (time->time[RTC_TIMETYPE_HOUR])*3600;
	ret *= 1000;
	ret += ms%1000;
	return ret;
}
