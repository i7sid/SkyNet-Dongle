/*
 * compass.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "compass.h"
#include "time.h"
#include "periph/adc.h"
#include "bitprint.h"
#include "./periph/led.h"
#include <math.h>

#define read_Addr 0x3D
#define write_Addr 0x3C
#define HMC5883L_Addr	0x1E // 0x3d or 0x3c >>1
#define PI	3.141592653589793

uint8_t Single_Mode[2] = {0x02,0x01};

RTC_TIME_T curTime;
RTC_TIME_T bufferedTime;

int xmax = -2500;
int xmin = 2500;
int ymax = -2500;
int ymin = 2500;

double xoffset = 0;
double yoffset = 0;

double scale = 1;

RTC_TIME_T starttime;
int caltime = 60; //timewindow for compass calibration


double readCompass(){
	DBG("Readcompass enter!\n");

	uint8_t readbuf[6]={0,0,0,0,0,0};
	//uint8_t cmd = 0x0A;
	//Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,&reg,1);
	//int ret =Chip_I2C_MasterRead(I2C0, HMC5883L_Addr, &buf, 6);
	//Chip_I2C_MasterCmdRead(I2C0,HMC5883L_Addr,cmd,&readbuf,6);
	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Single_Mode,2))!=2){
		DBG("Error I2C setting Single Mode\n");
	}
	msDelay(10);
	if((Chip_I2C_MasterRead(I2C0,HMC5883L_Addr,readbuf,6))!=6){
		DBG("Error I2C reading compass values\n");
	}

	uint8_t x1 = readbuf[0];
	uint8_t x2 = readbuf[1];
	uint8_t y1 = readbuf[2];
	uint8_t y2 = readbuf[3];
	uint16_t xaxis = (((x1 << 8) | x2)) ;
	uint16_t yaxis = (((y1 << 8) | y2)) ;
	int xstore = castuint16toint(xaxis);
	int ystore = castuint16toint(yaxis);
	DBG("%d %d \n",xstore,ystore);
	double x = xstore * 4.35; //gain correction
	double y = ystore * 4.35; //gain correction

	//calibration correction
	x += xoffset;
	y+= yoffset;
	y *= scale;


	double degs = 0;

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




void setupcompass(){
	DBG("Initialize Compass Modul...\n");

	//Board Setup
	Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);

	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0,100000);

	NVIC_DisableIRQ(I2C0 == I2C0 ? I2C0_IRQn : I2C1_IRQn);
	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandlerPolling);

	//HMC5883L init
	uint8_t Write_CRA [2]= {0x00,0x70};//default settings
	uint8_t Write_CRB [2]= {0x07,0xA0};//highest gain level
	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Write_CRA,2))!=2){
		DBG("Error I2C setting control register A\n");
	}
	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Write_CRB,2))!=2){
		DBG("Error I2C setting control register B\n");
	}

	DBG("Initialize Compass Modul complete...\n");
}

void calibcompass(){
	skynet_led_green(false);
	skynet_led_red(true);
	Chip_RTC_GetFullTime(LPC_RTC, &starttime);
	Chip_RTC_GetFullTime(LPC_RTC, &curTime);
	int starttimeh = timeAdd(&starttime);
	int curtimeh = timeAdd(&curTime);
	while((curtimeh - starttimeh) < caltime){
		uint8_t readbuf[6]={0,0,0,0,0,0};
		if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Single_Mode,2))!=2){
			DBG("Error I2C setting Single Mode\n");
		}
		msDelay(10);
		if((Chip_I2C_MasterRead(I2C0,HMC5883L_Addr,readbuf,6))!=6){
			DBG("Error I2C reading compass values\n");
		}

		uint8_t x1 = readbuf[0];
		uint8_t x2 = readbuf[1];
		uint8_t y1 = readbuf[4];
		uint8_t y2 = readbuf[5];
		uint16_t xaxis = (((x1 << 8) | x2)) ;
		uint16_t yaxis = (((y1 << 8) | y2)) ;
		int x = castuint16toint(xaxis);
		int y = castuint16toint(yaxis);

		curtimeh = timeAdd(&curTime);

		if(xmax < x){
			xmax = x;
		}
		if(xmin > x){
			xmin = x;
		}
		if(ymax < y){
			ymax = y;
		}
		if(ymin > y){
			ymin = y;
		}
	}
	skynet_led_green(true);
	skynet_led_red(false);
	xoffset = (xmin + xmax)/2;
	yoffset = (ymin + ymax)/2;
	double xlength = fabs(xmin)+fabs(xmax);
	double ylength = fabs(ymin)+fabs(ymax);
	scale = (xlength)/(ylength);

}

