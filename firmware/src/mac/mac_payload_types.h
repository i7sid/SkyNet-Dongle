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
  SENSOR_DATE           = 1, ///<@brief date of the sensor node
  SENSOR_POSITION       = 2, ///<@brief gps position data
  SENSOR_COMPASS        = 3, ///<@brief compass orientation
  SENSOR_WIND_SPEED     = 4, ///<@brief wind speed
  SENSOR_WIND_DIR       = 5, ///<@brief wind direction (compensated by compass)
  SENSOR_WIND_DIR_RAW   = 6, ///<@brief wind direction (uncompensated)
  SENSOR_HIST_WIND_SPEED_SHORT  = 7, ///<@brief
  SENSOR_HIST_WIND_DIR_SHORT    = 8, ///<@brief
  SENSOR_HIST_WIND_SPEED_LONG   = 9, ///<@brief
  SENSOR_HIST_WIND_DIR_LONG     = 10, ///<@brief
  SENSOR_HIST_WIND_SPEED_DIFF	= 11, ///<@brief
  SENSOR_HIST_WIND_DIR_DIFF		= 12  ///<@brief
} base_sensor_data_type;

typedef enum dongle_command {
	IGNORE				= 0,	///<@brief Nothing. Just ignore.
	TEST				= 1,	///<@brief Nothing. Just ACK.
	CALIB_COMPASS		= 2,	///<@brief Start the compass calibration.
	CALIB_COMPASS_STOP	= 3,	///<@brief Stop the compass calibration.
	BASE_START_SENDING	= 4,	///<@brief Start sending regular updates.
	BASE_STOP_SENDING	= 5,	///<@brief Stop sending regular updates.
	RF_SET_PARAMETERS	= 6		///<@brief Set parameters of RF communication.
} dongle_command;




#endif /* MAC_MAC_PAYLOAD_TYPES_H_ */
