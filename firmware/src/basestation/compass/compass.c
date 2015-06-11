/** @file
 * compass.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 *
 *      @brief Compass Module Functionality
 */

#include "compass.h"
#include "time.h"
#include "../../periph/adc.h"
#include "../tools/bitprint.h"
#include "../../periph/led.h"
#include <math.h>

#define read_Addr 0x3D
#define write_Addr 0x3C
#define HMC5883L_Addr	0x1E // 0x3d or 0x3c >>1
#define PI	3.141592653589793

uint8_t Single_Mode[2] = {0x02,0x01};

static uint32_t ticksPerSecond;

float xmax = -2500;
float xmin = 2500;
float ymax = -2500;
float ymin = 2500;

float xoffset = 0;
float yoffset = 0;

float scale = 1;


float readCompass(){
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
	uint8_t z1 = readbuf[2];
	uint8_t z2 = readbuf[3];
	uint16_t xaxis = (((x1 << 8) | x2)) ;
	uint16_t yaxis = (((y1 << 8) | y2)) ;
	uint16_t zaxis = (((z1 << 8) | z2)) ;

	float xstore = ((float) castuint16toint(xaxis))* 4.35; //gain correction
	float ystore = ((float) castuint16toint(yaxis))* 4.35; //gain correction
	float zstore = ((float) castuint16toint(zaxis))* 4.35; //gain correction

	xstore = sqrtf((xstore*xstore) - (zstore*zstore));
	ystore = sqrtf((ystore*ystore) - (zstore*zstore));
	DBG("%f %f \n",xstore,ystore);
	float x = xstore;
	float y = ystore;

	//calibration correction
	x += xoffset;
	y += yoffset;
	y *= scale;


	float degs = 0;

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




bool setupcompass(){
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
	uint8_t test [2] = {0xFF,0xFF};
	uint8_t Write_CRA [2]= {0x00,0x70};//default settings
	uint8_t Write_CRB [2]= {0x07,0xA0};//highest gain level

	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,test,2))!=2){
		DBG("Error sending test: Compass\n");
		return false;
	}

	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Write_CRA,2))!=2){
		DBG("Error I2C setting control register A\n");
	}
	if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Write_CRB,2))!=2){
		DBG("Error I2C setting control register B\n");
	}

	DBG("Initialize Compass Modul complete...\n");
	return true;
}

void calibcompass(){
	skynet_led_green(false);
	skynet_led_red(true);

	const uint32_t prescaleDivisor = 8;
	Chip_TIMER_Init(LPC_TIMER2);
	Chip_TIMER_PrescaleSet(LPC_TIMER2, prescaleDivisor - 1);
	Chip_TIMER_Enable(LPC_TIMER2);
	ticksPerSecond = Chip_Clock_GetSystemClockRate() / prescaleDivisor / 4;

	while((Chip_TIMER_ReadCount(LPC_TIMER2)/ticksPerSecond)< 60){

		uint8_t readbuf[6]={0,0,0,0,0,0};
		if((Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,Single_Mode,2))!=2){
			DBG("Error I2C setting Single Mode\n");
		}
		msDelayActive(10);
		if((Chip_I2C_MasterRead(I2C0,HMC5883L_Addr,readbuf,6))!=6){
			DBG("Error I2C reading compass values\n");
		}

		uint8_t x1 = readbuf[0];
		uint8_t x2 = readbuf[1];
		uint8_t y1 = readbuf[2];
		uint8_t y2 = readbuf[3];
		uint8_t z1 = readbuf[4];
		uint8_t z2 = readbuf[5];
		uint16_t xaxis = (((x1 << 8) | x2)) ;
		uint16_t yaxis = (((y1 << 8) | y2)) ;
		uint16_t zaxis = (((z1 << 8) | z2)) ;
		float x =((float) castuint16toint(xaxis))* 4.35; //gain correction
		float y =((float) castuint16toint(yaxis))* 4.35; //gain correction
		float z =((float) castuint16toint(zaxis))* 4.35; //gain correction

		DBG("%f %f %f\n",x,y,z);

		x = sqrtf((x*x) - (z*z));
		y = sqrtf((y*y) - (z*z));

		DBG("%f %f\n", x,y);


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
	float xlength = fabs(xmin)+fabs(xmax);
	float ylength = fabs(ymin)+fabs(ymax);
	scale = (xlength)/(ylength);

}
