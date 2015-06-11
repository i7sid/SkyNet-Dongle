/** @file
 *
 *
 *  @date 20.05.2015
 *  @author Christoph Kohl
 *
 *  @brief Functions for the UBX/NMEA-Protocol
 */

#ifndef BASESTATION_GPS_UBX_NMEA_PARSER_H_
#define BASESTATION_GPS_UBX_NMEA_PARSER_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

bool checksum(char * message);

bool checkAck(char * message);


#endif /* BASESTATION_GPS_UBX_NMEA_PARSER_H_ */
