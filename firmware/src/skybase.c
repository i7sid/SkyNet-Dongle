/*
 * skybase.c
 *
 *  Created on: 27.02.2015
 *      Author: christoph
 */
#include "skybase.h"
#include "./periph/adc.h"
#include "./misc/misc.h"
#include "./periph/adc.h"
#include "./radio/skynet_radio.h"
#include "./gpio_irq.h"
#include "./bitprint.h"
#include "./windspeedstatistics.h"
#include "./winddirectionstatistics.h"
#include "winddirection.h"
#include "time.h"
#include "compass.h"
#include "windspeed.h"
#include "periph/led.h"
#include <math.h>
#include <string.h>

uint32_t updaterate = 5000;

bool compassconnected = false;

RTC_TIME_T Time;


void baseinit(){
	DBG("Initialize Basestation Unit...\n");
	setupadc();
	setupirq();

		if (compassconnected){ //TODO use cpp
			setupcompass();
			calibcompass();
		}
	//	init_windspeed_statistics();
	//	init_winddir_statistics();
		 basedata_measure_start();
	DBG("Initialize Basestation Unit complete...\n");
}

void basedata_measure_start(){
	register_delayed_event(updaterate, (void*) updateBaseData);
}


void updateBaseData(){
	int dir = getWindDirection();
	if (compassconnected){//TODO use cpp
		//int dirMN = readCompass();
		//TODO compass correction of wind direction
	}
	int speed = calcwindspeed();
	//DBG("Basedata:\n Winddirection: %d deg; Windspeed: %d kmh \n", dir, speed);
	/* //FIXME statistics
	int** statdir = output_winddir_statistics();
	int ** statspeed = output_windspeed_statistics();

	for(int i = 0; i < 6; i++){
		DBG("Direction Level %d : %d %d %d\n",i,statdir[i][0],statdir[i][1],statdir[i][2]);
	}
	for(int i = 0; i < 6; i++){
			DBG("Speed Level %d : %d %d %d\n",i,statspeed[i][0],statspeed[i][1],statspeed[i][2]);
		}
	*/
	basedata_measure_start();
	unsigned int packet[2] = {(unsigned int)dir,(unsigned int)speed}; //TODO send statistics as well
	Chip_RTC_GetFullTime(LPC_RTC, &Time);
	DBG("Packet : %d %d :: %d:%d:%d \n"
	,packet[0],packet[1],Time.time[RTC_TIMETYPE_HOUR],Time.time[RTC_TIMETYPE_MINUTE],Time.time[RTC_TIMETYPE_SECOND]);
	uint8_t packet_send [8];
	memcpy(packet_send, packet, 8);
	skynet_led_blue(true);
	radio_send_variable_packet(packet_send, 8);
	skynet_led_blue(false);
}
