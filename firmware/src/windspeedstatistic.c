/*
 * windspeedectionstats.c
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

int windspeed_5sec[729];
int windspeed_15sec[243];
int windspeed_45sec[81];
int windspeed_2min15sec[27];
int windspeed_6min45sec[9];
int windspeed_20min15sec[3];
int windspeed_60min45sec[1];

int windspeed_5sec_counter = 0;
int windspeed_5sec_changes = 0;

int windspeed_15sec_counter = 0;
int windspeed_15sec_changes = 0;

int windspeed_45sec_counter = 0;
int windspeed_45sec_changes = 0;

int windspeed_2min15sec_counter = 0;
int windspeed_2min15sec_changes = 0;

int windspeed_6min45sec_counter = 0;
int windspeed_6min45sec_changes = 0;

int windspeed_20min15sec_counter = 0;
int windspeed_20min15sec_changes = 0;

int * output[7] = {windspeed_5sec,windspeed_15sec,
		windspeed_45sec,windspeed_2min15sec,
		windspeed_6min45sec,windspeed_20min15sec,
		windspeed_60min45sec};

void init_windspeed_statistics(){
	for(int i = 0; i < 729; i++){
		windspeed_5sec[i] = 0;
	}
}

void statistics_speed_level7(int value){
	windspeed_60min45sec[0] = value;
}

void statistics_speed_level6(int value){
	windspeed_20min15sec_counter++;
	windspeed_20min15sec_changes++;
	if(windspeed_20min15sec_counter < 3){ // array not full
		windspeed_20min15sec[windspeed_20min15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 3; i++){
			windspeed_20min15sec[i] = windspeed_20min15sec[i-1];
		}
		windspeed_20min15sec[0] = value;
	}
	if(windspeed_20min15sec_changes == 2){
		windspeed_20min15sec_changes = 0;
		int next = (1/3) * (windspeed_20min15sec[0]+windspeed_20min15sec[1]+windspeed_20min15sec[2]);
		statistics_speed_level7(next);
	}
}

void statistics_speed_level5(int value){
	windspeed_6min45sec_counter++;
	windspeed_6min45sec_changes++;
	if(windspeed_6min45sec_counter < 9){ // array not full
		windspeed_6min45sec[windspeed_6min45sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 9; i++){
			windspeed_6min45sec[i] = windspeed_6min45sec[i-1];
		}
		windspeed_6min45sec[0] = value;
	}
	if(windspeed_6min45sec_changes == 2){
		windspeed_6min45sec_changes = 0;
		int next = (1/3) * (windspeed_6min45sec[0]+windspeed_6min45sec[1]+windspeed_6min45sec[2]);
		statistics_speed_level6(next);
	}
}

void statistics_speed_level4(int value){
	windspeed_2min15sec_counter++;
	windspeed_2min15sec_changes++;
	if(windspeed_2min15sec_counter < 27){ // array not full
		windspeed_2min15sec[windspeed_2min15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 27; i++){
			windspeed_2min15sec[i] = windspeed_2min15sec[i-1];
		}
		windspeed_2min15sec[0] = value;
	}
	if(windspeed_2min15sec_changes == 2){
		windspeed_2min15sec_changes = 0;
		int next = (1/3) * (windspeed_2min15sec[0]+windspeed_2min15sec[1]+windspeed_2min15sec[2]);
		statistics_speed_level5(next);
	}
}

void statistics_speed_level3(int value){
	windspeed_45sec_counter++;
	windspeed_45sec_changes++;
	if(windspeed_45sec_counter < 81){ // array not full
		windspeed_45sec[windspeed_45sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 81; i++){
			windspeed_45sec[i] = windspeed_45sec[i-1];
		}
		windspeed_45sec[0] = value;
	}
	if(windspeed_45sec_changes == 2){
		windspeed_45sec_changes = 0;
		int next = (1/3) * (windspeed_45sec[0]+windspeed_45sec[1]+windspeed_45sec[2]);
		statistics_speed_level4(next);
	}
}

void statistics_speed_level2(int value){
	windspeed_15sec_counter++;
	windspeed_15sec_changes++;
	if(windspeed_15sec_counter < 243){ // array not full
		windspeed_15sec[windspeed_15sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 243; i++){
			windspeed_15sec[i] = windspeed_15sec[i-1];
		}
		windspeed_15sec[0] = value;
	}
	if(windspeed_15sec_changes == 2){
		windspeed_15sec_changes = 0;
		int next = (1/3) * (windspeed_15sec[0]+windspeed_15sec[1]+windspeed_15sec[2]);
		statistics_speed_level3(next);
	}
}

void statistics_speed_level1(int value){
	windspeed_5sec_counter++;
	windspeed_5sec_changes++;
	if(windspeed_5sec_counter < 729){ // array not full
		windspeed_5sec[windspeed_5sec_counter] = value;
	}else{ //array full shifting values
		for (int i = 1; i < 729; i++){
			windspeed_5sec[i] = windspeed_5sec[i-1];
		}
		windspeed_5sec[0] = value;
	}
	if(windspeed_5sec_changes == 2){
		windspeed_5sec_changes = 0;
		int next = (1/3) * (windspeed_5sec[0]+windspeed_5sec[1]+windspeed_5sec[2]);
		statistics_speed_level2(next);
	}
}

void input_windspeed_statistics(int value){
	statistics_speed_level1(value);
}

int ** output_windspeed_statistics(){
	return output;
}
