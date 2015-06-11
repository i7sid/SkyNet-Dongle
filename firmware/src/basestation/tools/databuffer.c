/*
 * databuffer.c
 *
 *  Created on: 20.05.2015
 *      Author: christoph
 */

#include "databuffer.h"
#include "../skybase.h"

#define bufferedtime 900
#define measuretime updaterate/1000
#define timeslots bufferedtime/measuretime

int dirbuffer[timeslots];
unsigned int dirwrite = 0;

int speedbuffer[timeslots];
unsigned int speedwrite = 0;


void putdir(int val){
	dirbuffer[dirwrite] = val;
	dirwrite++;
	if(dirwrite == timeslots) dirwrite = 0;
}

void putspeed(int val){
	speedbuffer[speedwrite] = val;
	speedwrite++;
	if(speedwrite == timeslots) speedwrite = 0;
}

int readspeedbuffer(unsigned int time){
	basedata_measure_stop();
	unsigned int curpointer = speedwrite;
	int slots = time/measuretime;
	int ret = 0;
	if(slots > timeslots){
		slots = timeslots;
	}
	for(int i = 0 ; i <= timeslots; i++){
		ret += speedbuffer[curpointer-i];
	}
	basedata_measure_start();
	return ret;
}

int readdirbuffer(unsigned int time){
	basedata_measure_stop();
	unsigned int curpointer = dirwrite;
	int slots = time/measuretime;
	int ret = 0;
	if(slots > timeslots){
		slots = timeslots;
	}
	for(int i = 0 ; i <= timeslots; i++){
		ret += speedbuffer[curpointer-i];
	}
	basedata_measure_start();
	return ret;
}
