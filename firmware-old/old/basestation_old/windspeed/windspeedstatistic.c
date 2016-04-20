/*
 * windspeedectionstats.c
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

#include "../../basestation_old/tools/statistics_tools.h"
#include "../../basestation_old/windspeed/windspeedstatistics.h"
#include "../../misc/debug.h"

#define nvals 3

int windspeed_5sec[nvals];
int windspeed_15sec[nvals];
int windspeed_45sec[nvals];
int windspeed_2min15sec[nvals];
int windspeed_6min45sec[nvals];
int windspeed_20min15sec[nvals];
int windspeed_60min45sec;

int windspeed_5sec_counter = 0;

int windspeed_15sec_counter = 0;

int windspeed_45sec_counter = 0;

int windspeed_2min15sec_counter = 0;

int windspeed_6min45sec_counter = 0;

int windspeed_20min15sec_counter = 0;

int * output[7] = {windspeed_5sec,windspeed_15sec,
		windspeed_45sec,windspeed_2min15sec,
		windspeed_6min45sec,windspeed_20min15sec,
		&windspeed_60min45sec};

void init_windspeed_statistics(){
	//nothing to do
}

void statistics_speed_clac(int value){
	DBG("level 0");
	shiftarray(windspeed_5sec,value, nvals);
	windspeed_5sec_counter++;
	if(windspeed_5sec_counter == (nvals-1)){
		DBG("level 1");
		windspeed_5sec_counter = 0;
		shiftarray(windspeed_15sec,1/3*(windspeed_5sec[0]+ windspeed_5sec[1]+ windspeed_5sec[2]), nvals);
		windspeed_15sec_counter++;
		if(windspeed_15sec_counter == (nvals-1)){
			DBG("level 2");
			windspeed_15sec_counter = 0;
			shiftarray(windspeed_45sec,1/3*(windspeed_15sec[0]+ windspeed_15sec[1]+ windspeed_15sec[2]), nvals);
			windspeed_45sec_counter++;
			if(windspeed_45sec_counter == (nvals-1)){
				DBG("level 3");
				windspeed_45sec_counter = 0;
				shiftarray(windspeed_2min15sec,1/3*(windspeed_45sec[0]+ windspeed_45sec[1]+ windspeed_45sec[2]), nvals);
				windspeed_2min15sec_counter++;
				if(windspeed_2min15sec_counter == (nvals-1)){
					DBG("level 4");
					windspeed_2min15sec_counter = 0;
					shiftarray(windspeed_6min45sec,1/3*(windspeed_2min15sec[0]+ windspeed_2min15sec[1]+ windspeed_2min15sec[2]), nvals);
					windspeed_6min45sec_counter++;
					if(windspeed_6min45sec_counter == (nvals-1)){
						windspeed_6min45sec_counter = 0;
						shiftarray(windspeed_20min15sec,1/3*(windspeed_6min45sec[0]+ windspeed_6min45sec[1]+ windspeed_6min45sec[2]), nvals);
						windspeed_20min15sec_counter++;
						if(windspeed_20min15sec_counter == (nvals-1)){
							windspeed_20min15sec_counter = 0;
							windspeed_60min45sec = 1/3*(windspeed_20min15sec[0]+ windspeed_20min15sec[1]+ windspeed_20min15sec[2]);
						}
					}
				}
			}
		}
	}
}
void input_windspeed_statistics(int value){
	statistics_speed_clac(value);
}

int ** output_windspeed_statistics(){
	return output;
}
