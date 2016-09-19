/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_COMPASS_COMPASS_H_
#define BASESTATION_COMPASS_COMPASS_H_

#include <stdlib.h>
#include <math.h>

#include "chip.h"
#include "../../misc/misc.h"
#include "../../misc/debug.h"
#include "../../cpu/systick.h"
#include <string.h>


/// @brief	I2C address of compass module.
#define HMC5883L_ADDR			0x1E 				// 0x3d or 0x3c >> 1

/// @brief	Value of mathematical constant PI.
#define PI						(3.141592653589793f)

/// @brief	Factor converting radians to degrees
#define COMPASS_RAD2DEGREE 		(57.2957795f)


// Taken from HMC5883L datasheet:
#define HMC5883L_CRA		(0x0)	///< @brief Configuration Register A
#define HMC5883L_CRB		(0x1)	///< @brief Configuration Register B
#define HMC5883L_MODE		(0x2)	///< @brief Mode Register
#define HMC5883L_DATA_X_H	(0x3)	///< @brief Data Output X MSB Register
#define HMC5883L_DATA_X_L	(0x4)	///< @brief Data Output X LSB Register
#define HMC5883L_DATA_Y_H	(0x5)	///< @brief Data Output Y MSB Register
#define HMC5883L_DATA_Y_L	(0x6)	///< @brief Data Output Y LSB Register
#define HMC5883L_DATA_Z_H	(0x7)	///< @brief Data Output Z MSB Register
#define HMC5883L_DATA_Z_L	(0x8)	///< @brief Data Output Z LSB Register
#define HMC5883L_STATUS		(0x9)	///< @brief Status Register
#define HMC5883L_ID_A		(0xA)	///< @brief Identification Register A
#define HMC5883L_ID_B		(0xB)	///< @brief Identification Register B
#define HMC5883L_ID_C		(0xC)	///< @brief Identification Register C


typedef struct compass_calibration_data {
	float offset_x;
	float offset_y;
	float offset_z;
	float factor_z;
	float declination;
} __attribute__((aligned(1),packed)) compass_calibration_data;

/**
 * @brief	Initializes the compass.
 */
int skynetbase_compass_init(void);


/**
 * @brief 	Read compass module.
 * @return 	Compass direction in degrees.
 */
float skynetbase_compass_read(void);


/**
 * @brief 	Old calibration routine.
 */
//void skynetbase_compass_calibrate(void);

/**
 * @brief 	Start calibration routine.
 */
void skynetbase_compass_start_calibration(void);

/**
 * @brief 	Stop calibration routine and calculate calibration.
 */
void skynetbase_compass_stop_calibration(void);

/**
 * @brief		Set the calibration configuration.
 * @param data	Pointer from where configuration will be \b copied.
 */
void skynetbase_compass_calibration_set(compass_calibration_data *data);

/**
 * @brief		Returns the calibration configuration.
 * @return		Pointer to internal calibration data.  (statically allocated memory)
 */
compass_calibration_data* skynetbase_compass_calibration_get(void);


#endif /* BASESTATION_COMPASS_COMPASS_H_ */
