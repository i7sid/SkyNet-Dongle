/** @file
 *
 *  @date 09.07.2015
 *  @author Christoph Kohl
 *	@brief Configurable Values
 */

#ifndef BASESTATION_OLD_CONFIG_H_
#define BASESTATION_OLD_CONFIG_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

// @brief Error of the earth's local magnetic field
int magneticdeclination;

// @brief Basestation Device ID
char * deviceID;

// @brief time of data being collected for one packet
unsigned int bufferedtime;

// @brief Sendingintervall1
unsigned int timeintervall1;

// @brief Sendingintervall2
unsigned int timeintervall2;

// @brief Updaterate of Data Sampling and Sending in MSec
unsigned int updaterate;

// @brief size of ADC buffer for winddirection values
unsigned int buffersize;

// @brief measures per second of winddirection data
uint32_t samplerate;

#endif /* BASESTATION_OLD_CONFIG_H_ */
