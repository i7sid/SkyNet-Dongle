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
#include "../config.h"

#define read_Addr 0x3D
#define write_Addr 0x3C
#define HMC5883L_Addr	0x1E // 0x3d or 0x3c >>1
#define PI	3.141592653589793
#define COMPASS_I2C	I2C2
#define COMPASS_I2C_IRQn	I2C2_IRQn
//#define COMPASS_I2C	I2C0
//#define COMPASS_I2C_IRQn	I2C0_IRQn
uint8_t Single_Mode[2] = {0x02,0x01};

static uint32_t ticksPerSecond;

float xmax = -2500;
float xmin = 2500;
float ymax = -2500;
float ymin = 2500;
float zmax = -2500;
float zmin = 2500;


float xoffset = 0;
float yoffset = 0;
float zoffset = 0;

float scaley = 1;
float scalez = 1;


float readCompass(){

	float degs = 0;
	//DBG("Readcompass enter!\n");
	//DBG("xoffset:%f yoffset:%f scale:%f",xoffset,yoffset,scale);
	//while(1){


	uint8_t readbuf[6]={0,0,0,0,0,0};
	//uint8_t cmd = 0x0A;
	//Chip_I2C_MasterSend(I2C0,HMC5883L_Addr,&reg,1);
	//int ret =Chip_I2C_MasterRead(I2C0, HMC5883L_Addr, &buf, 6);
	//Chip_I2C_MasterCmdRead(I2C0,HMC5883L_Addr,cmd,&readbuf,6);
	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,Single_Mode,2))!=2){
		DBG("Error I2C setting Single Mode\n");
	}
	msDelayActive(10);
	if((Chip_I2C_MasterRead(COMPASS_I2C,HMC5883L_Addr,readbuf,6))!=6){
		DBG("Error I2C reading compass values\n");
	}

	uint8_t x1 = readbuf[4];
	uint8_t x2 = readbuf[5];
	uint8_t y1 = readbuf[2];
	uint8_t y2 = readbuf[3];
	uint8_t z1 = readbuf[0];
	uint8_t z2 = readbuf[1];
	uint16_t xaxis = (((x1 << 8) | x2)) ;
	uint16_t yaxis = (((y1 << 8) | y2)) ;
	uint16_t zaxis = (((z1 << 8) | z2)) ;

	float xstore = ((float) castuint16toint(xaxis))*1.52;
	float ystore = ((float) castuint16toint(yaxis))*1.52;
	float zstore = ((float) castuint16toint(zaxis))*1.52;

	/*
	xstore = sqrtf((xstore*xstore) - (zstore*zstore));
	ystore = sqrtf((ystore*ystore) - (zstore*zstore));

	float x = xstore;
	float y = ystore;
	*/
	//calibration correction
	xstore -= xoffset;
	ystore -= yoffset;
	zstore -= zoffset;
	ystore *= scaley;
	zstore *= scalez;

	//DBG("%f %f %f %f \n",x,y,xstore,ystore);
	/*
		if(y > 0){
			degs = 90-((atan(x/y))*180 / PI);
		}

		else if(y < 0){
			degs = 270 -((atan(x/y))*180 / PI);
		}
		else if((y == 0) && (x < 0)){
			degs = 180;
		}

		else if ((y == 0) && (x > 0)){
			degs = 0;
		}

		else{
			DBG("atan2 Data Error\n");
		}
	//}
	 */

	float x = sqrtf((xstore*xstore) - (zstore*zstore));
	float y = sqrtf((ystore*ystore) - (zstore*zstore));

	degs = atan2(x,y);

	if(degs < 0)
		degs += 2*PI;

	if(degs > 2*PI)
		degs -= 2*PI;

	return (degs*180/PI)-(magneticdeclination/100);
}




bool setupcompass(){
	DBG("Initialize Compass Modul...\n");


	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);

	//Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
	//Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);

	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);

	Chip_I2C_Init(COMPASS_I2C);
	Chip_I2C_SetClockRate(COMPASS_I2C,100000);

	NVIC_DisableIRQ(COMPASS_I2C == COMPASS_I2C ? COMPASS_I2C_IRQn : I2C1_IRQn);
	Chip_I2C_SetMasterEventHandler(COMPASS_I2C, Chip_I2C_EventHandlerPolling);

	//HMC5883L init
	uint8_t test [2] = {0xFF,0xFF};
	uint8_t Write_CRA [2]= {0x00,0x70};//default settings
	uint8_t Write_CRB [2]= {0x03,0xA0};//gain level

	/*
	 * FIXED This sending test causes failure of commuication with compass on I2C2.
	 * Works fine on I2C0.
	 * not necessarily needed, so skipped on board version 3.1
	 *
	 * FIXME First try of Communication always fails.
	 */
	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,test,2))!=2){
		DBG("Error sending test1: Compass\n");
	}

	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,test,2))!=2){
		DBG("Error sending test2: Compass\n");
		return false;
	}

	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,Write_CRA,2))!=2){
		DBG("Error I2C setting control register A\n");
	}
	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,Write_CRB,2))!=2){
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
		if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,Single_Mode,2))!=2){
			DBG("Error I2C setting Single Mode\n");
		}
		msDelayActive(10);
		if((Chip_I2C_MasterRead(COMPASS_I2C,HMC5883L_Addr,readbuf,6))!=6){
			DBG("Error I2C reading compass values\n");
		}

		uint8_t x1 = readbuf[4];
		uint8_t x2 = readbuf[5];
		uint8_t y1 = readbuf[2];
		uint8_t y2 = readbuf[3];
		uint8_t z1 = readbuf[1];
		uint8_t z2 = readbuf[0];
		uint16_t xaxis = (((x1 << 8) | x2)) ;
		uint16_t yaxis = (((y1 << 8) | y2)) ;
		uint16_t zaxis = (((z1 << 8) | z2)) ;
		float x =((float) castuint16toint(xaxis))*1.52;
		float y =((float) castuint16toint(yaxis))*1.52;
		float z =((float) castuint16toint(zaxis))*1.52;


		//x = sqrtf((x*x) - (z*z));
		//y = sqrtf((y*y) - (z*z));

		//DBG("%f %f\n", x,y);


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
		if(zmax < z){
			zmax = z;
		}
		if(zmin > z){
			zmin = z;
		}

		send_compass_calibration_begin();
	}
	skynet_led_green(true);
	skynet_led_red(false);
	xoffset = (fabs(xmin)+fabs(xmax))/2;
	yoffset = (fabs(ymin)+fabs(ymax))/2;
	zoffset = (fabs(zmin)+fabs(zmax))/2;
	scaley = ((xoffset)/(yoffset));
	scalez = ((xoffset)/(zoffset));

	send_compass_calibration_end();

}

