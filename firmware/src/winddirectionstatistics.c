/*
 * winddirectionstatistics.c
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

#include <math.h>
#include "misc/debug.h"


#define nval 3

int winddir_5sec[nval];
int winddir_15sec[nval];
int winddir_45sec[nval];
int winddir_2min15sec[nval];
int winddir_6min45sec[nval];
int winddir_20min15sec[nval];
int winddir_60min45sec;

int winddir_5sec_counter = 0;

int winddir_15sec_counter = 0;

int winddir_45sec_counter = 0;

int winddir_2min15sec_counter = 0;

int winddir_6min45sec_counter = 0;

int winddir_20min15sec_counter = 0;

int * out[7] =
{winddir_5sec,winddir_15sec,
		winddir_45sec,winddir_2min15sec,
		winddir_6min45sec,winddir_20min15sec,
		&winddir_60min45sec};

void init_winddir_statistics(){
	//nothing to do
}



void statistics_dir_clac(int value){
	shiftarray(winddir_5sec,value, nval);
	winddir_5sec_counter++;
	if(winddir_5sec_counter == (nval-1)){
		winddir_5sec_counter = 0;
		shiftarray(winddir_15sec,addvec3(winddir_5sec[0], winddir_5sec[1], winddir_5sec[2]), nval);
		winddir_15sec_counter++;
		if(winddir_15sec_counter == (nval-1)){
			winddir_15sec_counter = 0;
			shiftarray(winddir_45sec,addvec3(winddir_15sec[0], winddir_15sec[1], winddir_15sec[2]), nval);
			winddir_45sec_counter++;
			if(winddir_45sec_counter == (nval-1)){
				winddir_45sec_counter = 0;
				shiftarray(winddir_2min15sec,addvec3(winddir_45sec[0], winddir_45sec[1], winddir_45sec[2]), nval);
				winddir_2min15sec_counter++;
				if(winddir_2min15sec_counter == (nval-1)){
					winddir_2min15sec_counter = 0;
					shiftarray(winddir_6min45sec,addvec3(winddir_2min15sec[0], winddir_2min15sec[1], winddir_2min15sec[2]), nval);
					winddir_6min45sec_counter++;
					if(winddir_6min45sec_counter == (nval-1)){
						winddir_6min45sec_counter = 0;
						shiftarray(winddir_20min15sec,addvec3(winddir_6min45sec[0], winddir_6min45sec[1], winddir_6min45sec[2]), nval);
						winddir_20min15sec_counter++;
						if(winddir_20min15sec_counter == (nval-1)){
							winddir_20min15sec_counter = 0;
							winddir_60min45sec = addvec3(winddir_20min15sec[0], winddir_20min15sec[1], winddir_20min15sec[2]);
						}
					}
				}
			}
		}
	}
}

void input_winddir_statistics(int value){
	statistics_dir_clac(value);
}

int ** output_winddir_statistics(){
	return out;
}

