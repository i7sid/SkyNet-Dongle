/** @file
 *
 *  @date 06.07.2015
 *  @author Christoph Kohl
 *	@brief Communication Protocol Interface
 */

#ifndef BASESTATION_COMMUNIKATION_COMPROT_H_
#define BASESTATION_COMMUNIKATION_COMPROT_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

void send_gps_nofix();

void send_gps_error(char * message, int length);

void send_compass_error(char * message, int length);

void send_compass_calibration_begin();

void send_compass_calibration_end();

void add_data(int speed, int dir);

void send_buffer();

void config_time();

void config_updatebasestation();

void config_updatewinddir();

void config_windspeed();

void send_mem_err();

#endif /* BASESTATION_COMMUNIKATION_COMPROT_H_ */
