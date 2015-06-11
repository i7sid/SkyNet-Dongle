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
#include "../radio/skynet_radio.h"
#include "../gpio//gpio_irq.h"
#include "./tools/bitprint.h"
#include "winddirection/winddirection.h"
#include "tools/time.h"
#include "compass/compass.h"
#include "windspeed/windspeed.h"
#include "../periph/led.h"
#include <math.h>
#include <string.h>
#include "gps/gpsmodule.h"

bool compassconnected = false;
bool gpsconnected = false;

RTC_TIME_T Time;

static uint32_t ticksPerSecond;
static uint32_t ticksPerMs;


void baseinit(){
	DBG("Initialize Basestation Unit...\n");
	//setupadc();
	//setupirq();

	compassconnected = setupcompass();
	if(!compassconnected){
		DBG("No Compass connected!\n");
	}else{
		calibcompass();
	}


	gpsconnected = gps_init();
	if(!gpsconnected){
		DBG("No GPS connected!\n");
	}else{
		struct gps_data * posandtime = NULL;
		posandtime = get_gps_data();
		if(posandtime->status == 0){
			DBG("GPS: No Fix!\n");
		}else{
		DBG("Time: %s\n",posandtime->utc);
		DBG("Position: %s %c :: %s %c\n\n",posandtime->lat,posandtime->north,
				posandtime->lon,posandtime->east);
		}
	}
	if(!compassconnected){
		if(!gpsconnected){
			Chip_I2C_DeInit(I2C0);
			DBG("Not a Basestation?\n");
			return;
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
		//TODO compass correction of wind direction
	}
	int speed = calcwindspeed();
	//DBG("Basedata:\n Winddirection: %d deg; Windspeed: %d kmh \n", dir, speed);
	basedata_measure_start();
	unsigned int packet[3] = {(unsigned int)dir,(unsigned int)speed,(unsigned int)dirMN};
	Chip_RTC_GetFullTime(LPC_RTC, &Time);
	uint32_t stoptime = Chip_TIMER_ReadCount(LPC_TIMER3)/ticksPerMs;
	Chip_TIMER_Reset(LPC_TIMER3);
	DBG("Packet : %d %d %d:: %d:%d:%d :: %d \n"
			,packet[0],packet[1],packet[2],Time.time[RTC_TIMETYPE_HOUR],Time.time[RTC_TIMETYPE_MINUTE],
			Time.time[RTC_TIMETYPE_SECOND],stoptime);
	uint8_t packet_send [9];
	memcpy(packet_send, packet, 9);
	skynet_led_blue(true);
	//radio_send_variable_packet(packet_send, 9);
	skynet_led_blue(false);

}
