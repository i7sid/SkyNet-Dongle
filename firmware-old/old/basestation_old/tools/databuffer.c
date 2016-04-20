/*
 * databuffer.c
 *
 *  Created on: 20.05.2015
 *      Author: christoph
 */

#include "../../basestation_old/tools/databuffer.h"

#include <stdlib.h>
#include "../../basestation_old/config.h"
#include "../../basestation_old/skybase.h"

unsigned int measuretime = 0;
unsigned int timeslots =  0;


//int dirbuffer[timeslots];
int * dirbuffer;
unsigned int dirwritepos = 0;
unsigned int writecounterdir = 0;

//int speedbuffer[timeslots];
int * speedbuffer;
unsigned int speedwritepos = 0;
unsigned int writecounterspeed = 0;

void init_data_buffers(){
	measuretime = updaterate/1000;
	timeslots =  bufferedtime/(measuretime);
	dirbuffer = malloc(timeslots*sizeof(int));
	speedbuffer = malloc(timeslots*sizeof(int));
	if((dirbuffer == NULL) || (speedbuffer == NULL)){
		//fixme error
	}
}

bool putdata(int speed, int dir){
	return (putspeed(speed) && putdir(dir));
}

void readdata(unsigned int time , int * speed, int * dir){
	basedata_measure_stop();
	*(speed) = readspeedbuffer(time);
	*(dir) = readdirbuffer(time);
	basedata_measure_start();
}

bool putdir(int val){
	dirbuffer[dirwritepos] = val;
	dirwritepos++;
	writecounterdir++;
	if(dirwritepos == timeslots) dirwritepos = 0;
	if(writecounterdir >= timeslots){
		return false;
	}else{
		return true;
	}
}

bool putspeed(int val){
	speedbuffer[speedwritepos] = val;
	speedwritepos++;
	writecounterspeed++;
	if(speedwritepos == timeslots) speedwritepos = 0;
	if(writecounterspeed >= timeslots){
		return false;
	}else{
		return true;
	}
}

int readspeedbuffer(unsigned int time){
	writecounterspeed = 0;
	unsigned int curpointer = speedwritepos;
	int slots = time/measuretime;
	int ret = 0;
	if(slots > timeslots){
		slots = timeslots;
	}
	int count = 0;
	while(count < slots){
		ret += speedbuffer[curpointer];
		curpointer--;
		if(curpointer < 0)curpointer = timeslots-1; //overflow
		count++;
	}
	return (ret/slots)*100;
}

int readdirbuffer(unsigned int time){
	writecounterdir = 0;
	unsigned int curpointer = dirwritepos;
	int slots = time/measuretime;
	float ret = 0;
	if(slots > timeslots){
		slots = timeslots;
	}
	int count = 0;
	while(count < slots){
		ret = addvec2(ret,dirbuffer[curpointer]);
		curpointer--;
		if(curpointer < 0)curpointer = timeslots-1; //overflow
		count++;
	}
	return (int)(ret*100);
}
