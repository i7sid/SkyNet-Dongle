/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "compass.h"

/// @brief Error of the earth's local magnetic field
float magneticdeclination = 0;

/*
int16_t xmax = COMPASS_UNCAL_MAX;
int16_t xmin = COMPASS_UNCAL_MIN;
int16_t ymax = COMPASS_UNCAL_MAX;
int16_t ymin = COMPASS_UNCAL_MIN;
int16_t zmax = COMPASS_UNCAL_MAX;
int16_t zmin = COMPASS_UNCAL_MIN;
*/

int16_t xmax = 403;
int16_t xmin = -57;
int16_t ymax = 39;
int16_t ymin = -379;
int16_t zmax = 252;
int16_t zmin = -199;


float xoffset = 0;
float yoffset = 0;
float zoffset = 0;

float scaley = 1;
float scalez = 1;


int skynetbase_compass_init(void) {
	DBG("Initialize Compass Modul...\n");

	Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);

	//Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
	//Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);

	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);

	Chip_I2C_Init(COMPASS_I2C);
	Chip_I2C_SetClockRate(COMPASS_I2C, 100000);

	NVIC_DisableIRQ(COMPASS_I2C == COMPASS_I2C ? COMPASS_I2C_IRQn : I2C1_IRQn);
	Chip_I2C_SetMasterEventHandler(COMPASS_I2C, Chip_I2C_EventHandlerPolling);

	//HMC5883L init
	/*
	uint8_t test [2] = {0xFF,0xFF};
	uint8_t Write_CRA [2]= {0x00, 0x70}; //default settings
	uint8_t Write_CRB [2]= {0x03, 0xA0}; //gain level
	*/

	/*
	 * FIXED This sending test causes failure of commuication with compass on I2C2.
	 * Works fine on I2C0.
	 * not necessarily needed, so skipped on board version 3.1
	 *
	 * FIXME First try of Communication always fails.
	 */

	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){0xFF,0xFF}, 2)) != 2) {
		//DBG("Error sending test1: Compass\n");
	}
	else {
		DBG("First compass try does not fail anymore. Hooray!");
	}

	/*
	if((Chip_I2C_MasterSend(COMPASS_I2C,HMC5883L_Addr,test,2))!=2){
		DBG("Error sending test2: Compass\n");
		return false;
	}
	*/

	/*
	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, Write_CRA, 2))!=2){
		DBG("Error I2C setting control register A\n");
	}
	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, Write_CRB, 2))!=2){
		DBG("Error I2C setting control register B\n");
	}
	*/

	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_CRA, 0x70}, 2);
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_CRB, 0xA0}, 2);
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_MODE, 0x0}, 2); // continuous, no I2C highspeed

	// test if calibration needed
	if (xmax < 0) {
		skynetbase_compass_calibrate();
	}

	DBG("Initialize Compass Modul complete...\n");
	return true;
}


/**
 * @brief	Read out compass values and return them in the given parameters.
 */
static void compass_read_raw(int16_t* x, int16_t* y, int16_t* z) {
	uint8_t readbuf[6] = {0, 0, 0, 0, 0, 0};

	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_MODE, 0x01}, 2)) != 2){
		DBG("Error I2C setting Single Mode\n");
	}

	msDelayActive(10);

	if((Chip_I2C_MasterRead(COMPASS_I2C, HMC5883L_Addr, readbuf, 6)) != 6) {
		DBG("Error I2C reading compass values\n");
	}

	*x = (readbuf[1] & 0xff) + ((int16_t)(readbuf[0]) << 8);
	*y = (readbuf[3] & 0xff) + ((int16_t)(readbuf[2]) << 8);
	*z = (readbuf[5] & 0xff) + ((int16_t)(readbuf[4]) << 8);
}

static float heading_to_degree(float heading) {
	// Correct for when signs are reversed.
	if (heading < 0) heading += 2*PI;

	// Check for wrap due to addition of declination.
	if (heading > 2*PI) heading -= 2*PI;

	// Convert radians to degrees for readability.
	return heading * 180 / PI;
}

// Source for calculation: http://bildr.org/2012/02/hmc5883l_arduino/
float skynetbase_compass_read(void) {
	//DBG("Readcompass enter!\n");
	//DBG("xoffset:%f yoffset:%f scale:%f",xoffset,yoffset,scale);
	//while(1){

	int16_t raw_x;
	int16_t raw_y;
	int16_t raw_z;
	compass_read_raw(&raw_x, &raw_y, &raw_z);

	DBG("%6d %6d %6d\n", raw_x, raw_y, raw_z);

	//float heading = atan2f(raw_y, raw_x);
	float heading = atan2f(raw_x, raw_y);
	DBG("%6.6f  %6.6f  %6.6f  %6.6f  %6.6f  %6.6f  \n",
			heading_to_degree(atan2f(raw_x, raw_y)),
			heading_to_degree(atan2f(raw_y, raw_z)),
			heading_to_degree(atan2f(raw_x, raw_z)),
			heading_to_degree(atan2f(raw_y, raw_x)),
			heading_to_degree(atan2f(raw_z, raw_x)),
			heading_to_degree(atan2f(raw_z, raw_y))
			);

	heading += magneticdeclination;


	return heading_to_degree(heading);
}

void skynetbase_compass_calibrate(void) {
	int count = 1000; // multiply with 20ms to get whole calibration time.
	DBG("Begin compass calibration...\n");

	// one iteration should take about 20ms.
	for (int i = 0; i < count; ++i) {
		int16_t raw_x;
		int16_t raw_y;
		int16_t raw_z;
		compass_read_raw(&raw_x, &raw_y, &raw_z);

		xmax = MAX(xmax, raw_x);
		ymax = MAX(ymax, raw_y);
		zmax = MAX(zmax, raw_z);
		xmin = MIN(xmin, raw_x);
		ymin = MIN(ymin, raw_y);
		zmin = MIN(zmin, raw_z);

		msDelayActive(10);
	}
	DBG("Finished compass calibration!\n");
	DBG("Results: %d < x < %d ;  %d < y < %d ;  %d < z < %d\n", xmin, xmax, ymin, ymax, zmin, zmax);
}
