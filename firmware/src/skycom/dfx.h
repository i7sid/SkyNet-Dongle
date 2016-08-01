/**
 * dfx is short for "dongle feather exchange" and enables the dongle to communicate with an adafruit feather.
 *
 *  @file
 *
 *  @date 	29.07.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SKYCOM_DFX_H_
#define SKYCOM_DFX_H_

#include "chip.h"

#define		DFX_RX_PORT		0
#define		DFX_RX_PIN 		9
#define		DFX_TX_PORT		0
#define		DFX_TX_PIN 		8


void dfx_init(void);


#endif /* SKYCOM_DFX_H_ */
