/*
 * statistics_tools.c
 *
 *  Created on: 07.05.2015
 *      Author: christoph
 */

#include "statistics_tools.h"
#include <math.h>

#define PI	3.141592653589793

void shiftarray(int * array, int value, int nval){
	for(int i = 1; i < nval; i++ ){
		array[i] = array[i-1];
	}
	array[0] = value;
}

int addvec3(int val1, int val2, int val3){

	double val1x = cos(val1 *(PI/180));
	double val1y = sin(val1 *(PI/180));

	double val2x = cos(val2 *(PI/180));
	double val2y = sin(val2 *(PI/180));

	double val3x = cos(val3 *(PI/180));
	double val3y = sin(val3 *(PI/180));

	return (int)(atan2(val1y+val2y+val3y,val1x+val2x+val3x));
}

int addvec2(int val1, int val2){

	double val1x = cos(val1 *(PI/180));
	double val1y = sin(val1 *(PI/180));

	double val2x = cos(val2 *(PI/180));
	double val2y = sin(val2 *(PI/180));

	double degs = 0;

	double x = val1x + val2x;
	double y = val1y +val2y;

	if(y > 0){
		degs = 90-((atan(x/y))*180 / PI);
	}

	if(y < 0){
		degs = 270 -((atan(x/y))*180 / PI);
	}
	if((y == 0) && (x < 0)){
		degs = 180;
	}

	if((y == 0) && (x > 0)){
		degs = 0;
	}

	return (int) degs;
}

float addvec2f(float val1, float val2){

	double val1x = cos(val1 *(PI/180));
	double val1y = sin(val1 *(PI/180));

	double val2x = cos(val2 *(PI/180));
	double val2y = sin(val2 *(PI/180));

	double degs = 0;

	double x = val1x + val2x;
	double y = val1y +val2y;

	if(y > 0){
		degs = 90-((atan(x/y))*180 / PI);
	}

	if(y < 0){
		degs = 270 -((atan(x/y))*180 / PI);
	}
	if((y == 0) && (x < 0)){
		degs = 180;
	}

	if((y == 0) && (x > 0)){
		degs = 0;
	}

	return degs;
}
