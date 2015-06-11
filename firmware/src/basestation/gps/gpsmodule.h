/** @file
 *
 *  @date 20.05.2015
 *  @author Christoph Kohl
 *	@brief GPS Module Interface
 */

#ifndef GPSMODUL_H_
#define GPSMODUL_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

struct gps_data{
	char utc[11];
	char lat[10];
	char north;
	char lon[11];
	char east;
	uint8_t status;
};

/**
 * @brief Setup GPS Module
 * @return \b true sucessfully connected \b false failure/ no Module connected
 *
 */
bool gps_init();

/**
 * @brief  Get GPS data
 *
 */
struct gps_data* get_gps_data();

/**
 * @brief Send ublox configuration
 */
void config_ublox();

void poll_messages();

void poll_one_message();

bool parse_message(char * message);

#endif /* GPSMODUL_H_ */
