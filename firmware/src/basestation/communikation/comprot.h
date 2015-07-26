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

//@brief Send gps nofix message
void send_gps_nofix();

//@brief Send gps error message
void send_gps_error(char * message, int length);

//@brief Send compass error
void send_compass_error(char * message, int length);

//@brief Send compass calibration begin message
void send_compass_calibration_begin();

//@brief Send compass calibration end message
void send_compass_calibration_end();

//@brief Send parameter value
void send_parameter_value(char * parameter, int value );

//@brief Send parameter values
void send_parameter_values(char * parameter, int value1, int value2);

//@brief add data to databuffers
void add_data(int speed, int dir);

//@brief Send data from buffer
void send_buffer();

//@brief configurate magnetic declination
void config_magneticdeclination(int declinationd);

//@brief configurate size of Databuffers
void config_databuffers(unsigned int buftime);

//@brief configurate send intervall1
void config_time1(int time1);

//@brief configurate send intervall2
void config_time2(int time2);

//@brief configurate system time
void config_time(uint32_t hour,uint32_t min,uint32_t sec);

//@brief configurate data collection rate
void config_updatebasestation(unsigned int rate);

//@brief configurate device ID
void config_deviceID(char * id);

//@brief configurate winddirection buffersize und samplerate
void config_updatewinddir(unsigned int adcbuffersize, unsigned int adcsamplerate);

//@brief build message header
void buildHeader(char * header);

//@brief Send malloc fail message
void send_mem_err();

//@brief get a config message
void get_config_message(char * message);

//@brief parse a config message
void parse_config_message(char * message);

//@brief encrypt a config message
void encrypt(char * message, char * encrypted);

//@brief decrypt a config message
void decrypt(char * encrypted, char * message);

#endif /* BASESTATION_COMMUNIKATION_COMPROT_H_ */
