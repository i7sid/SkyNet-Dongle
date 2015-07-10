/** @file
 *
 *  @date 06.07.2015
 *  @author Christoph Kohl
 *	@brief Communication Protocol Functionality
 */

#include "../../radio/skynet_radio.h"
#include "../communikation/comprot.h"
#include "../config.h"
#include "../tools/databuffer.h"
#include "../gps/gpsmodule.h"
#include <stdlib.h>
#include <string.h>

char * msgGpsNoFix = "GPS NoFix:";
char * msgGpsErr = "GPS Err:";
char * msgCompassErr = "Compass Err:";
char * msgCompassCalBegin = "Compass Calib. Start:";
char * msgCompassCalEnd = "Compass Calib. Success:";
char * msgDataPacket = "SkyBase Data:";
char * msgStatusPacket = "SkyBase Status:";
char * msgMemoryErr = "Err malloc failed";

void send_gps_nofix(){
	char * send = malloc(strlen(msgGpsNoFix)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,msgGpsNoFix);
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void send_gps_error(char * message, int length){
	char * send = malloc(strlen(msgGpsErr)+strlen(message)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,msgGpsErr);
	strcat(send,message);
	radio_send_variable_packet((uint8_t *)send,(uint16_t)strlen(send)+1);
	free(send);
	return;
}

void send_compass_error(char * message, int length){
	char * send = malloc(strlen(msgCompassErr)+strlen(message)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,msgCompassErr);
	strcat(send,message);
	radio_send_variable_packet((uint8_t *)send,(uint16_t)strlen(send)+1);
	free(send);
	return;
}

void send_compass_calibration_begin(){
	char * send = malloc(strlen(msgCompassCalBegin)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,msgCompassCalBegin);
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void send_compass_calibration_end(){
	char * send = malloc(strlen(msgCompassCalEnd)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,msgCompassCalEnd);
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void add_data(int speed, int dir){
	if(putdata(speed,dir)){
		return;
	}else{
		send_buffer();
		return;
	}
}

void send_buffer(){
	int sendspeed = 0;
	int senddir = 0;
	struct gps_data * pos = get_gps_data();
	readdata(sendtime,&sendspeed,&senddir);
	char * send = malloc(strlen(deviceID)+23+4+4+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	int position = strlen(deviceID);
	uint8_t speedtmp[4];
	memcpy(speedtmp,&sendspeed,4);
	uint8_t dirtmp[4];
	memcpy(dirtmp,&senddir,4);
	for(int i = 0; i < 10;i++){
		send[position] = pos->lat[i];
		position++;
	}
	send[position] = pos->north;
	position++;
	for(int i = 0; i < 11;i++){
		send[position] = pos->lon[i];
		position++;
	}
	send[position] = pos->east;
	position++;
	for(int i = 0; i < 4;i++){
		send[position] = speedtmp[i];
		position++;
	}
	for(int i = 0; i < 4;i++){
		send[position] = dirtmp[i];
		position++;
	}
	send[position] = '\0';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}
void config_time(){
	return;
}

void config_updatebasestation(){
	return;
}

void config_updatewinddir(){
	return;
}

void config_windspeed(){
	return;
}

void send_mem_err(){
	radio_send_variable_packet((uint8_t *)deviceID,(uint16_t)(strlen(deviceID)+1));
	radio_send_variable_packet((uint8_t *)msgMemoryErr,(uint16_t)(strlen(msgMemoryErr)+1));
}

