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



	// First try of Communication sometimes fails.
	if((Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){0xFF, 0xFF}, 2)) != 2) {
		// just ignore
	}

	//HMC5883L init

	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRA, 0x70}, 2);
	//Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0x40}, 2);  // Gain: 1,9Ga
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0x80}, 2);  // Gain: 4,0Ga
	//Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_CRB, 0xA0}, 2); // Gain: 4,7Ga
	Chip_I2C_MasterSend(COMPASS_I2C, HMC5883L_ADDR, (uint8_t[]){HMC5883L_MODE, 0x0}, 2); // continuous, no I2C highspeed


	DBG("Initialize Compass Modul complete...\n");

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

	// if you want to use 3d, you have to add another factor_y to scale to y axis
	//float scaled_x = (float)raw_x - calib.offset_x;
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

	DBG("%d\t%d\t%d\t%6.3f\t%6.3f\t%6.3f\n", raw_y, raw_x, raw_z, scaled_y, scaled_z, bearing);

	return bearing;
}

static void compass_calib_worker(void) {
	if (!calibrating) return;

	int16_t compass_x = 0;
	int16_t compass_y = 0;
	int16_t compass_z = 0;

	compass_read_raw(&compass_x, &compass_y, &compass_z);

	float compass_x_scaled = (float)compass_x;
	float compass_y_scaled = (float)compass_y;
	float compass_z_scaled = (float)compass_z;


	if (compass_x_scaled > calib_x_max) calib_x_max = compass_x_scaled;
	if (compass_x_scaled < calib_x_min) calib_x_min = compass_x_scaled;
	if (compass_y_scaled > calib_y_max) calib_y_max = compass_y_scaled;
	if (compass_y_scaled < calib_y_min) calib_y_min = compass_y_scaled;
	if (compass_z_scaled > calib_z_max) calib_z_max = compass_z_scaled;
	if (compass_z_scaled < calib_z_min) calib_z_min = compass_z_scaled;

	//DBG("calib: %d %d %d\n", compass_x, compass_y, compass_z);

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
	DBG("Starting calibration.\n");
	calibrating = true;
	register_delayed_event(200, compass_calib_worker);
}

void skynetbase_compass_stop_calibration(void) {
	DBG("Finishing calibration.\n");
	remove_delayed_event(compass_calib_worker);
	if (!calibrating) return;

	calibrating = false;

	__disable_irq();
	// calibration calculation
	calib.offset_x = (calib_x_max + calib_x_min) * 0.5;
	calib.offset_y = (calib_y_max + calib_y_min) * 0.5;
	calib.offset_z = (calib_z_max + calib_z_min) * 0.5;
	calib.factor_z = (calib_y_max - calib_y_min) / (calib_z_max - calib_z_min);
	__enable_irq();
}


