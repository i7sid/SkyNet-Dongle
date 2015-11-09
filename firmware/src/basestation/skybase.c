/** @file
 *
 *  @date 27.02.2015
 *  @Author: Christoph Kohl
 *
 *  @brief Functionality of the Basestation Unit
 */
#include "skybase.h"
#include "../periph/adc.h"
#include "../misc/misc.h"
#include "../periph/adc.h"
#include "../gpio//gpio_irq.h"
#include "./tools/bitprint.h"
#include "winddirection/winddirection.h"
#include "tools/time.h"
#include "compass/compass.h"
#include "windspeed/windspeed.h"
#include "../periph/led.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "communikation/comprot.h"
#include "gps/gpsmodule.h"
#include "config.h"

bool compassconnected = false;
bool gpsconnected = false;

RTC_TIME_T Time;

static uint32_t ticksPerSecond;
static uint32_t ticksPerMs;


void baseinit(){
	DBG("Initialize Basestation Unit...\n");
	setupadc();
	setupirq();

	compassconnected = setupcompass();
	if(!compassconnected){
		char * errormessage = "No Compass connected!";
		send_compass_error(errormessage,(int)strlen(errormessage));
	}else{
		calibcompass();
	}

	gpsconnected = gps_init();
	if(!gpsconnected){
		char * errormessage = "No GPS connected!";
		send_gps_error(errormessage,(int)strlen(errormessage));
	}else{
		struct gps_data defaultpos =
		{
				"0123456789",
				"012345678",
				"a",
				"0123456789",
				"b",
				"1"
		};
		struct gps_data * posandtime = &defaultpos;
		while(true){
			posandtime = get_gps_data();
			if(strcmp(posandtime->status,"0")==0){
				send_gps_nofix();
				while(!poll_one_message()){};//poll new message
			}else{
				DBG("Time: %s\n",posandtime->utc);
				DBG("Position: %s %s :: %s %s\n\n",posandtime->lat,posandtime->north,
						posandtime->lon,posandtime->east);
				char hours[3] = {posandtime->utc[0],posandtime->utc[1],'\0'};
				char mins[3] = {posandtime->utc[2],posandtime->utc[3],'\0'};
				char secs[3] = {posandtime->utc[4],posandtime->utc[5],'\0'};
				uint32_t hour = atoi(hours);
				uint32_t min =  atoi(mins);
				uint32_t sec = atoi(secs);
				Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_HOUR,hour);
				Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_MINUTE,min);
				Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_SECOND,sec);
				shutdown_GPS();
				break;
			}

		}
	}


	if(!compassconnected){
		if(!gpsconnected){
			DBG("Not a Basestation?\n");
			//return;
		}

	}
	basedata_measure_start();

	const uint32_t prescaleDivisor = 8;
	Chip_TIMER_Init(LPC_TIMER3);
	Chip_TIMER_PrescaleSet(LPC_TIMER3, prescaleDivisor - 1);
	Chip_TIMER_Enable(LPC_TIMER3);


	ticksPerSecond = Chip_Clock_GetSystemClockRate() / prescaleDivisor / 4;
	ticksPerMs = ticksPerSecond / 1000;


	DBG("Initialize Basestation Unit complete...\n");
}

void basedata_measure_start(){
	register_delayed_event(updaterate, (void*) updateBaseData);
}

void basedata_measure_stop(){
	remove_delayed_event((void*) updateBaseData);
}

void updateBaseData(){
	int dir = getWindDirection();
	int dirMN = 0;
	if (compassconnected){
		dirMN = readCompass();
	}
	int speed = calcwindspeed();
	basedata_measure_start();
	unsigned int packet[3] = {(unsigned int)dir,(unsigned int)speed,(unsigned int)dirMN};
	Chip_RTC_GetFullTime(LPC_RTC, &Time);
	uint32_t stoptime = Chip_TIMER_ReadCount(LPC_TIMER3)/ticksPerMs;
	Chip_TIMER_Reset(LPC_TIMER3);
	DBG("Packet : %d %d %d:: %d:%d:%d :: %d \n"
			,packet[0],packet[1],packet[2],Time.time[RTC_TIMETYPE_HOUR],Time.time[RTC_TIMETYPE_MINUTE],
			Time.time[RTC_TIMETYPE_SECOND],stoptime);
	add_data(speed,dir);
	//skynet_led_blue(true);
	//skynet_led_blue(false);

}
