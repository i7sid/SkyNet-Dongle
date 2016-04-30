/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "compass.h"

static float calib_x_min = 1000;
static float calib_x_max = -1000;
static float calib_y_min = 1000;
static float calib_y_max = -1000;
static float calib_z_min = 1000;
static float calib_z_max = -1000;
static compass_calibration_data calib;
static bool calibrating = false;

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


	// First try of Communication always fails.
	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){0xFF, 0xFF}, 2)) != 2) {
		//DBG("Error sending test1: Compass\n");
	}
	else {
		DBG("First compass try does not fail anymore. Hooray!");
	}

	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRA, 0x70}, 2);
	//Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0x40}, 2);  // Gain: 1,9Ga
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0x80}, 2);  // Gain: 4,0Ga
	//Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0xA0}, 2); // Gain: 4,7Ga
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_MODE, 0x0}, 2); // continuous, no I2C highspeed


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

	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_MODE, 0x01}, 2)) != 2){
		DBG("Error I2C setting Single Mode\n");
	}

	msDelayActive(10);

	if((Chip_I2C_MasterRead(COMPASS_I2C, HMC5883L_ADDR, readbuf, 6)) != 6) {
		DBG("Error I2C reading compass values\n");
	}

	// data is sent by compass in order X.msb, X.lsb, Z.msb, Z.lsb, Y.msb, Y.lsb !
	*x = (readbuf[1] & 0xff) + ((int16_t)(readbuf[0]) << 8);
	*y = (readbuf[5] & 0xff) + ((int16_t)(readbuf[4]) << 8);
	*z = (readbuf[3] & 0xff) + ((int16_t)(readbuf[2]) << 8);
}

// Source for calculation: http://bildr.org/2012/02/hmc5883l_arduino/
float skynetbase_compass_read(void) {
	int16_t raw_x;
	int16_t raw_y;
	int16_t raw_z;
	compass_read_raw(&raw_x, &raw_y, &raw_z);

	//DBG("X: %6d; Y: %6d; Z: %6d\n", raw_x, raw_y, raw_z);

	//float scaled_x = (float)raw_x * compass_gain_fact * (float)compass_cal_x_gain + (float)COMPASS_CAL_X_OFFSET;
	/*
	float scaled_y = (float)raw_y * COMPASS_GAIN_FACT * (float)COMPASS_CAL_Y_GAIN + (float)COMPASS_CAL_Y_OFFSET;
	float scaled_z = (float)raw_z * COMPASS_GAIN_FACT * (float)COMPASS_CAL_Z_GAIN + (float)COMPASS_CAL_Z_OFFSET;
	*/

	/*
	float scaled_y = (float)raw_y * COMPASS_GAIN_FACT * COMPASS_CAL_Y_GAIN - calib.offset_y;
	float scaled_z = (float)raw_z * COMPASS_GAIN_FACT * COMPASS_CAL_Z_GAIN - calib.offset_z;
	*/

	float scaled_y = (float)raw_y - calib.offset_y;
	float scaled_z = ((float)raw_z - calib.offset_z) * calib.factor_z;


	//DBG("Y : %6.3f; Z : %6.3f\n", (float)raw_y, (float)raw_z);

	float bearing = 0;
	if (scaled_y > 0) {
		bearing = 270 - atan(scaled_z / scaled_y) * COMPASS_RAD2DEGREE;
	}
	else if (scaled_y < 0) {
		bearing = 90 - atan(scaled_z / scaled_y) * COMPASS_RAD2DEGREE;
	}
	else if (scaled_y == 0 && scaled_z < 0) {
		bearing = 0;
	}
	else {
		bearing = 180;
	}

	bearing += calib.declination;		// compensate magnetic declination

	// bring into range 0°..360°
	if (bearing > 360.0f) {
		bearing -= 360.0f;
	}
	else if (bearing < 0.0f) {
		bearing += 360.0f;
	}

	DBG("%d\t%d\t%6.3f\t%6.3f\t%6.3f\n", raw_y, raw_x, scaled_y, scaled_z, bearing);

	return bearing;
}

static void compass_calib_worker(void) {
	if (!calibrating) return;

	int16_t compass_x = 0;
	int16_t compass_y = 0;
	int16_t compass_z = 0;
	/*
	float compass_x_offset = 0;
	float compass_y_offset = 0;
	float compass_z_offset = 0;
	*/
	float compass_x_scaled = 0;
	float compass_y_scaled = 0;
	float compass_z_scaled = 0;

	compass_read_raw(&compass_x, &compass_y, &compass_z);
/*
	compass_x_scaled = (float)compass_x * COMPASS_GAIN_FACT * COMPASS_CAL_X_GAIN;
	compass_y_scaled = (float)compass_y * COMPASS_GAIN_FACT * COMPASS_CAL_Y_GAIN;
	compass_z_scaled = (float)compass_z * COMPASS_GAIN_FACT * COMPASS_CAL_Z_GAIN;
	*/
	compass_x_scaled = (float)compass_x;
	compass_y_scaled = (float)compass_y;
	compass_z_scaled = (float)compass_z;

	//DBG("%d\t%d\t%d\t%d\t%d\t%f\n", compass_x, compass_y - 88, compass_z - 99, compass_y, compass_z, (float)compass_z * 0.779886);
	//DBG("%f\t%f\t%f\n", compass_x_scaled, compass_y_scaled, compass_z_scaled);

	if (compass_x_scaled > calib_x_max) calib_x_max = compass_x_scaled;
	if (compass_x_scaled < calib_x_min) calib_x_min = compass_x_scaled;
	if (compass_y_scaled > calib_y_max) calib_y_max = compass_y_scaled;
	if (compass_y_scaled < calib_y_min) calib_y_min = compass_y_scaled;
	if (compass_z_scaled > calib_z_max) calib_z_max = compass_z_scaled;
	if (compass_z_scaled < calib_z_min) calib_z_min = compass_z_scaled;

	// reschedule
	register_delayed_event(125, compass_calib_worker);
}

void skynetbase_compass_calibration_set(compass_calibration_data *data) {
	memcpy(&calib, data, sizeof(compass_calibration_data));
}

compass_calibration_data* skynetbase_compass_calibration_get(void) {
	return &calib;
}

void skynetbase_compass_start_calibration(void) {
	DBG("Starting calibration...\n");
	calibrating = true;
	register_delayed_event(200, compass_calib_worker);
}

void skynetbase_compass_stop_calibration(void) {
	remove_delayed_event(compass_calib_worker);
	if (!calibrating) return;

	calibrating = false;

	__disable_irq();
	// calibration calculation
	calib.offset_x = (calib_x_max + calib_x_min) * 0.5;
	calib.offset_y = (calib_y_max + calib_y_min) * 0.5;
	calib.offset_z = (calib_z_max + calib_z_min) * 0.5;
	calib.factor_z = (calib_y_max + calib_y_min) / (calib_z_max + calib_z_min);
	__enable_irq();

	DBG("-----------------------------------\n");
	DBG("Stopping calibration, calculated:\n");
	DBG("min: %f|%f|%f\n", calib_x_min, calib_y_min, calib_z_min);
	DBG("max: %f|%f|%f\n", calib_x_max, calib_y_max, calib_z_max);
	DBG("off: %f|%f|%f\n", calib.offset_x, calib.offset_y, calib.offset_z);
	DBG("fact: %f\n", calib.factor_z);
	DBG("-----------------------------------\n\n");
}



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
//void skynetbase_compass_calibrate(void) {
//	// ***********************************************************
//	// offset_calibration() function performs two taskes
//	// 1. It calculates the diffrence in the gain of the each axis magnetometer axis, using
//	//    inbuilt self excitation function of HMC5883L (Which is useless if it is used as a compass
//	//    unless you are very unlucy and got a crapy sensor or live at very High or low temperature)
//	// 2. It calculates the mean of each axes magnetic field, when the Magnetometer is rotated 360 degree
//	// 3. Do Both
//	// ***********************************************************
//
//	int16_t compass_x = 0;
//	int16_t compass_y = 0;
//	int16_t compass_z = 0;
//	float compass_x_offset = 0;
//	float compass_y_offset = 0;
//	float compass_z_offset = 0;
//	float compass_x_scaled = 0;
//	float compass_y_scaled = 0;
//	float compass_z_scaled = 0;
//	float compass_x_gainError = 0;
//	float compass_y_gainError = 0;
//	float compass_z_gainError = 0;
//	//float bearing = 0;
//
//	int select = 3;
//
//	// *****************************************************************************************
//	// Gain offset estimation
//	// *****************************************************************************************
//	if (select == 1 || select == 3) { // User input in the function
//		// Configuring the Control register for Positive Bais mode
//		DBG("Calibrating the Magnetometer: Gain\n");
//		Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){0x0, 0b01110001}, 2);
//
//		compass_read_raw(&compass_x, &compass_y, &compass_z); // Disregarding the first data
//
//		// Reading the Positive baised Data
//		while (compass_x < 200 || compass_y < 200 || compass_z < 200) {   // Making sure the data is with Positive baised
//			compass_read_raw(&compass_x, &compass_y, &compass_z);
//		}
//
//		compass_x_scaled = compass_x * COMPASS_GAIN_FACT;
//		compass_y_scaled = compass_y * COMPASS_GAIN_FACT;
//		compass_z_scaled = compass_z * COMPASS_GAIN_FACT;
//
//
//		// Offset = 1160 - Data_positive
//		compass_x_gainError = (float)COMPASS_XY_EXCITATION / compass_x_scaled;
//		compass_y_gainError = (float)COMPASS_XY_EXCITATION / compass_y_scaled;
//		compass_z_gainError = (float)COMPASS_Z_EXCITATION / compass_z_scaled;
//
//
//		// Configuring the Control register for Negative Bais mode
//		Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){0x0, 0b01110010}, 2);
//
//		compass_read_raw(&compass_x, &compass_y, &compass_z); // Disregarding the first data
//		// Reading the Negative baised Data
//		while (compass_x > -200 || compass_y > -200 || compass_z > -200) {   // Making sure the data is with negative baised
//			compass_read_raw(&compass_x, &compass_y, &compass_z);
//		}
//
//		compass_x_scaled = compass_x * COMPASS_GAIN_FACT;
//		compass_y_scaled = compass_y * COMPASS_GAIN_FACT;
//		compass_z_scaled = compass_z * COMPASS_GAIN_FACT;
//
//
//		// Taking the average of the offsets
//		compass_x_gainError = (float)((COMPASS_XY_EXCITATION / abs(compass_x_scaled)) + compass_x_gainError) / 2;
//		compass_y_gainError = (float)((COMPASS_XY_EXCITATION / abs(compass_y_scaled)) + compass_y_gainError) / 2;
//		compass_z_gainError = (float)((COMPASS_Z_EXCITATION / abs(compass_z_scaled)) + compass_z_gainError) / 2;
//
//
//		DBG("x_gain_offset = %f\n", compass_x_gainError);
//		DBG("y_gain_offset = %f\n", compass_y_gainError);
//		DBG("z_gain_offset = %f\n", compass_z_gainError);
//	}
//
//	// Configuring the Control register for normal mode
//	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){0x0, 0b01111000}, 2);
//
//
//	// *****************************************************************************************
//	// Offset estimation
//	// *****************************************************************************************
//	if (select == 2 || select == 3) { // User input in the function
//		DBG("Calibrating the Magnetometer: Offset\n");
//		DBG("Please rotate the magnetometer 2 or 3 times in complete circules with in 30 seconds!\n");
//
//		for (uint8_t i = 0; i < 10; ++i){   // Disregarding first few data
//			compass_read_raw(&compass_x, &compass_y, &compass_z);
//		}
//
//		float x_max = -4000, y_max = -4000, z_max = -4000;
//		float x_min = 4000, y_min = 4000, z_min = 4000;
//
//		/*
//		Debug code ------------------------------
//
//		if (compass_debug == 1){
//			DBGln("Starting Debug data in ");
//			delay(1000);
//			DBGln("3");
//			delay(1000);
//			DBGln("2");
//			delay(1000);
//			DBGln("1");
//			delay(1000);
//			DBGln("0");
//			DBGln();
//			for(byte i=0;i<10;i++){
//				DBG("*");
//			}
//			DBGln("*");
//			DBGln("Debug -- (Offset Calibration)");
//			for(byte i=0;i<10;i++){
//				DBG("*");
//			}
//			DBGln("*");
//		}
//    	// End Debug code */
//
//
//		uint16_t cnt = 0;
//		while (cnt < 270) {
//			compass_read_raw(&compass_x, &compass_y, &compass_z);
//
//			compass_x_scaled = (float)compass_x * COMPASS_GAIN_FACT * compass_x_gainError;
//			compass_y_scaled = (float)compass_y * COMPASS_GAIN_FACT * compass_y_gainError;
//			compass_z_scaled = (float)compass_z * COMPASS_GAIN_FACT * compass_z_gainError;
//
//			/*
//			if (compass_debug == 1){  //------------------ Debug Data
//				DBG(compass_x_scaled);
//				DBG("\t");
//				DBG(compass_y_scaled);
//				DBG("\t");
//				DBGln(compass_z_scaled);
//			}//--------------------------------- End Debug Data
//			*/
//
//			x_max = MAX(x_max, compass_x_scaled);
//			y_max = MAX(y_max, compass_y_scaled);
//			z_max = MAX(z_max, compass_z_scaled);
//
//			x_min = MIN(x_min, compass_x_scaled);
//			y_min = MIN(y_min, compass_y_scaled);
//			z_min = MIN(z_min, compass_z_scaled);
//
//			msDelayActive(100);
//			cnt++;
//		}
//
//		/*
//		Debug code ------------------------------
//
//		if (compass_debug == 1){
//		DBGln();
//		for(byte i=0;i<10;i++){
//		 DBG("*");
//		}
//		DBGln("*");
//		DBGln("End Debug -- (Offset Calibration)");
//		for(byte i=0;i<10;i++){
//		 DBG("*");
//		}
//		DBGln("*");
//		}
//		// End Debug code */
//
//		compass_x_offset = ((x_max-x_min)/2)-x_max;
//		compass_y_offset = ((y_max-y_min)/2)-y_max;
//		compass_z_offset = ((z_max-z_min)/2)-z_max;
//
//		DBG("Offset x  = %f mG\n", compass_x_offset);
//		DBG("Offset y  = %f mG\n", compass_y_offset);
//		DBG("Offset z  = %f mG\n", compass_z_offset);
//	}
//
//	DBG("Calibration finished. Waiting 10 seconds...\n");
//	msDelayActive(10000);
//}
