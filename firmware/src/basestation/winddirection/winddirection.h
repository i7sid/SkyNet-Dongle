/**	@file
 *
 * 	@date 26.04.2015
 *	@author Christoph Kohl
 *
 *	@brief Interface of the Windsensor Module for Winddirection
 */

#ifndef WINDDIRECTION_H_
#define WINDDIRECTION_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/**
 *	@brief get measured Winddirection
 *	@return Winddirection in degs with compass correction
 */
int getWindDirection();

/**
 *	@brief get measured Winddirection
 *	@return Winddirection in degs without compass correction
 */
int getWindDirection_raw();

/**
 * @brief Enqueue winvane_measure() in Eventqueue
 */
void winvane_measure_start();

/**
 * @brief Dequeue winvane_measure() in Eventqueue
 */
void winvane_measure_stop();

/**
 * 	@brief Measure Winddirection
 */
void winvane_measure();

/**
 * 	@brief Setup Winddirection Measure
 */
void setupadc();


#endif /* WINDDIRECTION_H_ */
