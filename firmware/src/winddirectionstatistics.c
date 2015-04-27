/*
 * winddirectionstatistics.c
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

#include <math.h>

#define PI	3.141592653589793

int winddir_5sec[729];
int winddir_15sec[243];
int winddir_45sec[81];
int winddir_2min15sec[27];
int winddir_6min45sec[9];
int winddir_20min15sec[3];
int winddir_60min45sec[1];

int winddir_5sec_counter = 0;
int winddir_5sec_changes = 0;

int winddir_15sec_counter = 0;
int winddir_15sec_changes = 0;

int winddir_45sec_counter = 0;
int winddir_45sec_changes = 0;

int winddir_2min15sec_counter = 0;
int winddir_2min15sec_changes = 0;

int winddir_6min45sec_counter = 0;
int winddir_6min45sec_changes = 0;

int winddir_20min15sec_counter = 0;
int winddir_20min15sec_changes = 0;

int * out[7] =
		{winddir_5sec,winddir_15sec,
		winddir_45sec,winddir_2min15sec,
		winddir_6min45sec,winddir_20min15sec,
		winddir_60min45sec};

void init_winddir_statistics(){
	for(int i = 0; i < 729; i++){
		winddir_5sec[i] = 0;
	}
}

int addvec(int val1, int val2, int val3){

	double val1x = cos(val1 *(PI/180));
	double val1y = sin(val1 *(PI/180));

	double val2x = cos(val2 *(PI/180));
	double val2y = sin(val2 *(PI/180));

	double val3x = cos(val3 *(PI/180));
	double val3y = sin(val3 *(PI/180));

	return (int)(atan2(val1y+val2y+val3y,val1x+val2x+val3x));
}

void statistics_level7(int value){
	winddir_60min45sec[0] = value;
}

void statistics_level6(int value){
	winddir_20min15sec_counter++;
	winddir_20min15sec_changes++;
	if(winddir_20min15sec_counter < 3){ // array not full
		winddir_20min15sec[winddir_20min15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 3; i++){
			winddir_20min15sec[i] = winddir_20min15sec[i-1];
		}
		winddir_20min15sec[0] = value;
	}
	if(winddir_20min15sec_changes == 2){
		winddir_20min15sec_changes = 0;
		int next = (1/3) * addvec(winddir_20min15sec[0],winddir_20min15sec[1],winddir_20min15sec[2]);
		statistics_level7(next);
	}
}

void statistics_level5(int value){
	winddir_6min45sec_counter++;
	winddir_6min45sec_changes++;
	if(winddir_6min45sec_counter < 9){ // array not full
		winddir_6min45sec[winddir_6min45sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 9; i++){
			winddir_6min45sec[i] = winddir_6min45sec[i-1];
		}
		winddir_6min45sec[0] = value;
	}
	if(winddir_6min45sec_changes == 2){
		winddir_6min45sec_changes = 0;
		int next = (1/3) * addvec(winddir_6min45sec[0],winddir_6min45sec[1],winddir_6min45sec[2]);
		statistics_level6(next);
	}
}

void statistics_level4(int value){
	winddir_2min15sec_counter++;
	winddir_2min15sec_changes++;
	if(winddir_2min15sec_counter < 27){ // array not full
		winddir_2min15sec[winddir_2min15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 27; i++){
			winddir_2min15sec[i] = winddir_2min15sec[i-1];
		}
		winddir_2min15sec[0] = value;
	}
	if(winddir_2min15sec_changes == 2){
		winddir_2min15sec_changes = 0;
		int next = (1/3) * addvec(winddir_2min15sec[0],winddir_2min15sec[1],winddir_2min15sec[2]);
		statistics_level5(next);
	}
}

void statistics_level3(int value){
	winddir_45sec_counter++;
	winddir_45sec_changes++;
	if(winddir_45sec_counter < 81){ // array not full
		winddir_45sec[winddir_45sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 81; i++){
			winddir_45sec[i] = winddir_45sec[i-1];
		}
		winddir_45sec[0] = value;
	}
	if(winddir_45sec_changes == 2){
		winddir_45sec_changes = 0;
		int next = (1/3) * addvec(winddir_45sec[0],winddir_45sec[1],winddir_45sec[2]);
		statistics_level4(next);
	}
}

void statistics_level2(int value){
	winddir_15sec_counter++;
	winddir_15sec_changes++;
	if(winddir_15sec_counter < 243){ // array not full
		winddir_15sec[winddir_15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 243; i++){
			winddir_15sec[i] = winddir_15sec[i-1];
		}
		winddir_15sec[0] = value;
	}
	if(winddir_15sec_changes == 2){
		winddir_15sec_changes = 0;
		int next = (1/3) * addvec(winddir_15sec[0],winddir_15sec[1],winddir_15sec[2]);
		statistics_level3(next);
	}
}

void statistics_level1(int value){
	winddir_5sec_counter++;
	winddir_5sec_changes++;
	if(winddir_5sec_counter < 729){ // array not full
		winddir_5sec[winddir_5sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 729; i++){
			winddir_5sec[i] = winddir_5sec[i-1];
		}
		winddir_5sec[0] = value;
	}
	if(winddir_5sec_changes == 2){
		winddir_5sec_changes = 0;
		int next = (1/3) * addvec(winddir_5sec[0],winddir_5sec[1],winddir_5sec[2]);
		statistics_level2(next);
	}
}

void input_winddir_statistics(int value){
	statistics_level1(value);
}

int ** output_winddir_statistics(){
	return out;
}

