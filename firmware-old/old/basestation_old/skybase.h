/** @file
 *
 *  @date 16.01.2015
 *  @author Christoph Kohl
 *
 *  @brief Defines the Main Interface of the Basestation Unit
 */

#ifndef SKYBASE_H_
#define SKYBASE_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/**
 * @brief Collects and sends sampled Data
 */
void updateBaseData();

/**
 * @brief Initiates Basestation Unit
 */
void baseinit();

/**
 * @brief Dequeues updateBaseData() from Eventqueue
 */
void basedata_measure_stop();

/**
 * @brief Enqueues updateBaseData() from Eventqueue
 */
void basedata_measure_start();

#endif /* SKYBASE_H_ */
