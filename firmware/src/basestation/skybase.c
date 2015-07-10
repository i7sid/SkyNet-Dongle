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

#include "communikation/comprot.h"
#include "gps/gpsmodule.h"

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
		struct gps_data * posandtime = NULL;
		/*{
				{'0','1','2','3','4','5','6','7','8','9','0'},
				{'0','1','2','3','4','5','6','7','8','9'},
				'N',
				{'0','1','2','3','4','5','6','7','8','9','0'},
				'E',
				1;
		};
		*/
		posandtime = get_gps_data();
		if(strcmp(posandtime->status,"0")==0){
			send_gps_nofix();//todo poll new message
		}else{
		DBG("Time: %s\n",posandtime->utc);
		DBG("Position: %s %s :: %s %s\n\n",posandtime->lat,posandtime->north,
				posandtime->lon,posandtime->east);
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
	skynet_led_blue(true);
	skynet_led_blue(false);

}
