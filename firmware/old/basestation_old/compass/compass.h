/** @file
 *
 *  @date 26.04.2015
 *  @author Christoph Kohl
 *
 *  @brief Compass Module Interface
 */

#ifndef COMPASS_H_
#define COMPASS_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/**
 * @brief Calibration of the Compass Module
 */
void calibcompass();

/**
 * @brief Setup Compass Module
 * @return \b true sucessfully connected \b false failure/ no Module connected
 * @note detection if connected only works with pullups on I2c bus
 */
bool setupcompass();

/**
 * @brief Read Compass Module
 * @return Compass Direction in degs
 */

float readCompass();

#endif /* COMPASS_H_ */
