/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_COMPASS_COMPASS_H_
#define BASESTATION_COMPASS_COMPASS_H_

#include <math.h>

#include "chip.h"
#include "../../misc/debug.h"
#include "../../cpu/systick.h"


#define read_Addr 			0x3D				// 0b111101
#define write_Addr 			0x3C				// 0b111100
#define HMC5883L_Addr		0x1E 				// 0x3d or 0x3c >> 1
#define PI					(float)3.141592653589793
#define COMPASS_I2C			I2C2
#define COMPASS_I2C_IRQn	I2C2_IRQn

#define COMPASS_UNCAL_MAX	-32766
#define COMPASS_UNCAL_MIN	32766


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
 * @brief 	Start calibration routine.
 */
void skynetbase_compass_calibrate(void);


#endif /* BASESTATION_COMPASS_COMPASS_H_ */
