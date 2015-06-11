/*
 * bitprint.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "./bitprint.h"

void printBin32littleE(uint32_t n){
	for(int i = 0; i < 32; i++) {
		if (n & 1)
			printf("1");
		else
			printf("0");

		n >>= 1;

	}
	printf("\n");
}

void printBin32(uint32_t n){
	for(int i = 31; i >=0; i--) {
		if ((n & (1 << i)) >> i)
			printf("1");
		else
			printf("0");


	}
	printf("\n");
}

void printBin16(uint16_t n){
	for(int i = 15; i >=0; i--) {
		if ((n & (1 << i)) >> i)
			printf("1");
		else
			printf("0");


	}
	printf("\n");
}

void printBin16littleE(uint16_t n){
	for(int i = 0; i < 16; i++ ) {
		if (n & 1)
			printf("1");
		else
			printf("0");

		n >>= 1;
	}
	printf("\n");

}

int castuint16toint(uint16_t n){ //ugly stuff but necessary ;)
	int ret = 0;
	if ((n & (1 << 15)) >> 15){ //16th bit is 1
		ret |= n;
		ret |=  0XFFFF << 16; //set int sign bits

	}else{ //16th bit is 0
		ret |= n;
	}
	return ret;
}

