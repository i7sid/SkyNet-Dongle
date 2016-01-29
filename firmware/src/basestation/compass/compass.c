/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "compass.h"

// TODO?
#define compass_XY_excitation 	(1160) // The magnetic field excitation in X and Y direction during Self Test (Calibration)
#define compass_Z_excitation 	(1080) // The magnetic field excitation in Z direction during Self Test (Calibration)
#define compass_rad2degree 		(57.3)

#define compass_cal_x_gain 		(1.014098)    // Stored Gain offset at room temperature
#define compass_cal_y_gain 		(1.037909)    // Stored Gain offset at room temperature
#define compass_cal_z_gain 		(0.997668)    // Stored Gain offset at room temperature
#define compass_cal_x_offset 	-428		// -428.172363 mG
#define compass_cal_y_offset 	-104		// -103.666321 mG
#define compass_cal_z_offset 	465			// 465.397278 mG


//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/// @brief Error of the earth's local magnetic field
float magneticdeclination = 0;

float compass_gain_fact = 2.27; // for Gain: 4.0Ga
//float compass_gain_fact = 2.56; // for Gain: 4.7Ga


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

	Chip_IOCON_PinMux(LPC_IOCON, COMPASS_I2C_SDA_PORT, COMPASS_I2C_SDA_PIN, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, COMPASS_I2C_SCL_PORT, COMPASS_I2C_SCL_PIN, IOCON_MODE_INACT, IOCON_FUNC2);

	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);

	Chip_I2C_Init(COMPASS_I2C);
	Chip_I2C_SetClockRate(COMPASS_I2C, 100000);

	NVIC_DisableIRQ(COMPASS_I2C == COMPASS_I2C ? COMPASS_I2C_IRQn : I2C1_IRQn);
	Chip_I2C_SetMasterEventHandler(COMPASS_I2C, Chip_I2C_EventHandlerPolling);

	//HMC5883L init


	// FIXME First try of Communication always fails.
	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){0xFF, 0xFF}, 2)) != 2) {
		//DBG("Error sending test1: Compass\n");
	}
	else {
		DBG("First compass try does not fail anymore. Hooray!");
	}

	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_CRA, 0x70}, 2);
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_CRB, 0x80}, 2);  // Gain: 4,0Ga
	//Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_CRB, 0xA0}, 2); // Gain: 4,7Ga
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){HMC5883L_MODE, 0x0}, 2); // continuous, no I2C highspeed

	// test if calibration needed
	if (xmax < 0) {
		skynetbase_compass_calibrate();
	}

	DBG("Initialize Compass Modul complete...\n");

	// DEBUG
	//skynetbase_compass_calibrate();

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

	// data is sent by compass in order X.msb, X.lsb, Z.msb, Z.lsb, Y.msb, Y.lsb !
	*x = (readbuf[1] & 0xff) + ((int16_t)(readbuf[0]) << 8);
	*y = (readbuf[5] & 0xff) + ((int16_t)(readbuf[4]) << 8);
	*z = (readbuf[3] & 0xff) + ((int16_t)(readbuf[2]) << 8);
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
	int16_t raw_x;
	int16_t raw_y;
	int16_t raw_z;
	compass_read_raw(&raw_x, &raw_y, &raw_z);

	//DBG("X: %6d; Y: %6d; Z: %6d\n", raw_x, raw_y, raw_z);

	float scaled_x = (float)raw_x * compass_gain_fact * (float)compass_cal_x_gain + (float)compass_cal_x_offset;
	float scaled_y = (float)raw_y * compass_gain_fact * (float)compass_cal_y_gain + (float)compass_cal_y_offset;
	float scaled_z = (float)raw_z * compass_gain_fact * (float)compass_cal_z_gain + (float)compass_cal_z_offset;

	DBG("X: %6.3f; Y: %6.3f; Z: %6.3f\n", scaled_x, scaled_y, scaled_z);

	float bearing = 0;
	if (scaled_y > 0) {
		bearing = 90 - atan(scaled_z / scaled_y) * compass_rad2degree;
	}
	else if (scaled_y < 0) {
		bearing = 270 - atan(scaled_z / scaled_y) * compass_rad2degree;
	}
	else if (scaled_y == 0 && scaled_z < 0) {
		bearing = 180;
	}
	else {
		bearing = 0;
	}
/*
	float bearing = 0;
	if (scaled_z > 0) {
		bearing = 90 - atan(scaled_x/scaled_z) * compass_rad2degree;
	}
	else if (scaled_z < 0) {
		bearing = 270 - atan(scaled_x / scaled_z) * compass_rad2degree;
	}
	else if (scaled_z == 0 && scaled_x < 0) {
		bearing = 180;
	}
	else {
		bearing = 0;
	}
 * */
	return bearing;

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

/*
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
*/


/**
 * @brief 	Compass calibration.
 * @Source	https://github.com/helscream/HMC5883L_Header_Arduino_Auto_calibration
 *
 * This Function calculates the offset in the Magnetometer
 * using Positive and Negative bias Self test capability
 * This function updates X_offset Y_offset and Z_offset Global variables
 *
 * Call skynet_compass_init() before.
 */
void skynetbase_compass_calibrate(void) {
	// ***********************************************************
	// offset_calibration() function performs two taskes
	// 1. It calculates the diffrence in the gain of the each axis magnetometer axis, using
	//    inbuilt self excitation function of HMC5883L (Which is useless if it is used as a compass
	//    unless you are very unlucy and got a crapy sensor or live at very High or low temperature)
	// 2. It calculates the mean of each axes magnetic field, when the Magnetometer is rotated 360 degree
	// 3. Do Both
	// ***********************************************************

	int16_t compass_x = 0;
	int16_t compass_y = 0;
	int16_t compass_z = 0;
	float compass_x_offset = 0;
	float compass_y_offset = 0;
	float compass_z_offset = 0;
	float compass_x_scalled = 0;
	float compass_y_scalled = 0;
	float compass_z_scalled = 0;
	float compass_x_gainError = 0;
	float compass_y_gainError = 0;
	float compass_z_gainError = 0;
	//float bearing = 0;

	int select = 3;

	// *****************************************************************************************
	// Gain offset estimation
	// *****************************************************************************************
	if (select == 1 || select == 3) { // User input in the function
		// Configuring the Control register for Positive Bais mode
		DBG("Calibrating the Magnetometer: Gain\n");
		Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){0x0, 0b01110001}, 2);

		compass_read_raw(&compass_x, &compass_y, &compass_z); // Disregarding the first data

		// Reading the Positive baised Data
		while (compass_x < 200 || compass_y < 200 || compass_z < 200) {   // Making sure the data is with Positive baised
			compass_read_raw(&compass_x, &compass_y, &compass_z);
		}

		compass_x_scalled = compass_x * compass_gain_fact;
		compass_y_scalled = compass_y * compass_gain_fact;
		compass_z_scalled = compass_z * compass_gain_fact;


		// Offset = 1160 - Data_positive
		compass_x_gainError = (float)compass_XY_excitation / compass_x_scalled;
		compass_y_gainError = (float)compass_XY_excitation / compass_y_scalled;
		compass_z_gainError = (float)compass_Z_excitation / compass_z_scalled;


		// Configuring the Control register for Negative Bais mode
		Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){0x0, 0b01110010}, 2);

		compass_read_raw(&compass_x, &compass_y, &compass_z); // Disregarding the first data
		// Reading the Negative baised Data
		while (compass_x > -200 || compass_y > -200 || compass_z > -200) {   // Making sure the data is with negative baised
			compass_read_raw(&compass_x, &compass_y, &compass_z);
		}

		compass_x_scalled = compass_x * compass_gain_fact;
		compass_y_scalled = compass_y * compass_gain_fact;
		compass_z_scalled = compass_z * compass_gain_fact;


		// Taking the average of the offsets
		compass_x_gainError = (float)((compass_XY_excitation / abs(compass_x_scalled)) + compass_x_gainError) / 2;
		compass_y_gainError = (float)((compass_XY_excitation / abs(compass_y_scalled)) + compass_y_gainError) / 2;
		compass_z_gainError = (float)((compass_Z_excitation / abs(compass_z_scalled)) + compass_z_gainError) / 2;


		DBG("x_gain_offset = %f\n", compass_x_gainError);
		DBG("y_gain_offset = %f\n", compass_y_gainError);
		DBG("z_gain_offset = %f\n", compass_z_gainError);
	}

	// Configuring the Control register for normal mode
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_Addr, (uint8_t[]){0x0, 0b01111000}, 2);
	// bit configuration = 0 A A DO2 DO1 DO0 MS1 MS2

	/*
	A A                        DO2 DO1 DO0      Sample Rate [Hz]      MS1 MS0    Measurment Mode
	0 0 = No Average            0   0   0   =   0.75                   0   0   = Normal
	0 1 = 2 Sample average      0   0   1   =   1.5                    0   1   = Positive Bias
	1 0 = 4 Sample Average      0   1   0   =   3                      1   0   = Negative Bais
	1 1 = 8 Sample Average      0   1   1   =   7.5                    1   1   = -
							   1   0   0   =   15 (Default)
							   1   0   1   =   30
							   1   1   0   =   75
							   1   1   1   =   -
	*/


	// *****************************************************************************************
	// Offset estimation
	// *****************************************************************************************
	if (select == 2 || select == 3) { // User input in the function
		DBG("Calibrating the Magnetometer: Offset\n");
		DBG("Please rotate the magnetometer 2 or 3 times in complete circules with in 30 seconds!\n");

		for (uint8_t i = 0; i < 10; ++i){   // Disregarding first few data
			compass_read_raw(&compass_x, &compass_y, &compass_z);
		}

		float x_max = -4000, y_max = -4000, z_max = -4000;
		float x_min = 4000, y_min = 4000, z_min = 4000;

		/*
		Debug code ------------------------------

		if (compass_debug == 1){
			DBGln("Starting Debug data in ");
			delay(1000);
			DBGln("3");
			delay(1000);
			DBGln("2");
			delay(1000);
			DBGln("1");
			delay(1000);
			DBGln("0");
			DBGln();
			for(byte i=0;i<10;i++){
				DBG("*");
			}
			DBGln("*");
			DBGln("Debug -- (Offset Calibration)");
			for(byte i=0;i<10;i++){
				DBG("*");
			}
			DBGln("*");
		}
    	// End Debug code */


		uint16_t cnt = 0;
		while (cnt < 270) {
			compass_read_raw(&compass_x, &compass_y, &compass_z);

			compass_x_scalled = (float)compass_x * compass_gain_fact * compass_x_gainError;
			compass_y_scalled = (float)compass_y * compass_gain_fact * compass_y_gainError;
			compass_z_scalled = (float)compass_z * compass_gain_fact * compass_z_gainError;

			/*
			if (compass_debug == 1){  //------------------ Debug Data
				DBG(compass_x_scalled);
				DBG("\t");
				DBG(compass_y_scalled);
				DBG("\t");
				DBGln(compass_z_scalled);
			}//--------------------------------- End Debug Data
			*/

			x_max = MAX(x_max, compass_x_scalled);
			y_max = MAX(y_max, compass_y_scalled);
			z_max = MAX(z_max, compass_z_scalled);

			x_min = MIN(x_min, compass_x_scalled);
			y_min = MIN(y_min, compass_y_scalled);
			z_min = MIN(z_min, compass_z_scalled);

			msDelayActive(100);
			cnt++;
		}

		/*
		Debug code ------------------------------

		if (compass_debug == 1){
		DBGln();
		for(byte i=0;i<10;i++){
		 DBG("*");
		}
		DBGln("*");
		DBGln("End Debug -- (Offset Calibration)");
		for(byte i=0;i<10;i++){
		 DBG("*");
		}
		DBGln("*");
		}
		// End Debug code */

		compass_x_offset = ((x_max-x_min)/2)-x_max;
		compass_y_offset = ((y_max-y_min)/2)-y_max;
		compass_z_offset = ((z_max-z_min)/2)-z_max;

		DBG("Offset x  = %f mG\n", compass_x_offset);
		DBG("Offset y  = %f mG\n", compass_y_offset);
		DBG("Offset z  = %f mG\n", compass_z_offset);
	}

	DBG("Calibration finished. Waiting 10 seconds...\n");
	msDelayActive(10000);
}
