/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef BASESTATION_SKYNET_BASESTATION_H_
#define BASESTATION_SKYNET_BASESTATION_H_


/**
 * @brief	Initializes all components of the basestation.
 */
void skynetbase_init(void);

/**
 * @brief	Starts collecting and sending the collected data.
 */
void skynetbase_start_measurement(void);

/**
 * @brief	Stops collection and sending data.
 */
void skynetbase_stop_measurement(void);


#endif /* BASESTATION_SKYNET_BASESTATION_H_ */
