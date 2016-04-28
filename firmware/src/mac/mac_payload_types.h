/**
 *  @file
 *
 *  @date 28.04.2016
 *  @author Micheal Zapf <michael.zapf@fau.de>
 */

#ifndef MAC_MAC_PAYLOAD_TYPES_H_
#define MAC_MAC_PAYLOAD_TYPES_H_


typedef enum mac_payload_type {
	ETHERFRAME 		= 1,	///<@brief Tunneled ethernet frame
	BASESENSORDATA 	= 2		///<@brief Sensor data from base station
} mac_payload_type;


typedef enum base_sensor_data_type {
	SENSOR_DATE 	= 0x1,	///<@brief date of the sensor node
	SENSOR_POSITION	= 0x2,	///<@brief gps position data
	SENSOR_COMPASS	= 0x4,	///<@brief compass orientation
	SENSOR_WIND    	= 0x8	///<@brief wind speed and direction
} base_sensor_data_type;




#endif /* MAC_MAC_PAYLOAD_TYPES_H_ */
