/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_GPS_GPS_H_
#define BASESTATION_GPS_GPS_H_

#include <string.h>
#include <stdlib.h>
#include "chip.h"
#include "../../misc/misc.h"
#include "../../misc/debug.h"
#include "../../misc/event_queue.h"
#include "../../cpu/systick.h"


#define GPS_UART 			LPC_UART1
#define GPS_IRQn			UART1_IRQn
#define GPS_IRQ_Handler		UART1_IRQHandler

#define GPS_UART_INT_MASK 	(UART_IER_RBRINT)


typedef struct gps_pubx_data {
	char	time[7];	///< @brief Current time
	double	lat;		///< @brief Latitude
	char	lat_dir;	///< @brief	N or S
	double	lon;		///< @brief Longitude
	char	lon_dir;	///< @brief	E or W
	double	alt;		///< @brief Altitude
	char	status;
	float	accuracy_horizontal;
	float	accuracy_vertical;
	uint8_t	satellites;

} gps_pubx_data;


/**
 * @brief	Initializes the gps module.
 */
int skynetbase_gps_init(void);

/**
 * @brief	Returns a pointer to the last received position data.
 */
gps_pubx_data* skynetbase_gps_get_data(void);

/**
 * @brief	Sends a configuration message to the gps module.
 */
void skynetbase_gps_config(char msg[]);

/**
 * @brief	Retrieves data from gps module.
 *
 * Must be called regularily
 */
void skynetbase_gps_receive_data(void);

/**
 * @brief	Parses the received string.
 */
void skynetbase_gps_received_data(char gpsout[], uint8_t gpsoutpos);


/**
 * @brief	Queries for GPS data.
 */
void skynetbase_gps_query(void);


#endif /* BASESTATION_GPS_GPS_H_ */
