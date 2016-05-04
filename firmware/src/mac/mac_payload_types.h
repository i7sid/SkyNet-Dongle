/**
 *  @file
 *
 *  @date 28.04.2016
 *  @author Micheal Zapf <michael.zapf@fau.de>
 */

#ifndef MAC_MAC_PAYLOAD_TYPES_H_
#define MAC_MAC_PAYLOAD_TYPES_H_


/**
 * This is not transmitted in frames!
 * (Given implicitly by which headers are sent.)
 */
typedef enum mac_payload_type {
	NONE	 			= 0,	///<@brief No type specified. Normally ignore.
	ETHERFRAME 			= 1,	///<@brief Tunneled ethernet frame
	BASE_SENSOR_DATA 	= 2,	///<@brief Sensor data from base station
	DONGLE_CMD	 		= 3,	///<@brief Command to dongle
	DONGLE_CMD_ANSWER	= 4		///<@brief Answer from dongle (to command)
} mac_payload_type;


typedef enum base_sensor_data_type {
	SENSOR_DATE 		= 0x1,	///<@brief date of the sensor node
	SENSOR_POSITION		= 0x2,	///<@brief gps position data
	SENSOR_COMPASS		= 0x4,	///<@brief compass orientation
	SENSOR_WIND_SPEED   = 0x8,	///<@brief wind speed
	SENSOR_WIND_DIR		= 0x10	///<@brief wind direction (uncompensated)
} base_sensor_data_type;


typedef enum dongle_command {
	IGNORE				= 0,	///<@brief Nothing. Just ignore.
	TEST				= 1,	///<@brief Nothing. Just ACK.
	CALIB_COMPASS		= 2,	///<@brief Start the compass calibration.
	CALIB_COMPASS_STOP	= 3,	///<@brief Stop the compass calibration.
	BASE_START_SENDING	= 4,	///<@brief Start sending regular updates.
	BASE_STOP_SENDING	= 5		///<@brief Stop sending regular updates.
} dongle_command;




#endif /* MAC_MAC_PAYLOAD_TYPES_H_ */
