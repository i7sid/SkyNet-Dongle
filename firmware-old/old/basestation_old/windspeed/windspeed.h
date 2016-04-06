/** @file
 *
 * 	@date 26.04.2015
 *  @author Christoph Kohl
 *
 *  @brief Interface of the Windsensor Module for Windspeed
 */

#ifndef WINDSPEED_H_
#define WINDSPEED_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/**
 * @brief Windspeed calculation
 * @return Windspeed in 100m/h
 */
int calcwindspeed();

/**
 * @brief Interrupthandler for counting rotations
 */
void tickhandler();

/**
 * @brief setup Windspeed Measure
 */
void setupirq();


#endif /* WINDSPEED_H_ */
