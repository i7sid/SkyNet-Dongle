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
#include "../winddirection/winddirection.h"
#include <stdlib.h>
#include <string.h>

RTC_TIME_T time;

#define messagelength 101

char key[31] = "123456789987654321123456789987";
char password[8] = "geheim";

char * head;
char * msgGpsNoFix = ":GPS NoFix:";
char * msgGpsErr = ":GPS Err:";
char * msgCompassErr = ":Compass Err:";
char * msgCompassCalBegin = ":Compass Calib. Start:";
char * msgCompassCalEnd = ":Compass Calib. Success:";
char * msgDataPacket = ":SkyBase Data:";
char * msgStatusPacket = ":SkyBase Status:";
char * msgMemoryErr = ":Err malloc failed:";

void send_gps_nofix(){
	buildHeader(head);
	char * send = malloc(strlen(msgGpsNoFix)+strlen(deviceID)+strlen(head)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgGpsNoFix);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void send_gps_error(char * message, int length){
	buildHeader(head);
	char * send = malloc(strlen(head)+strlen(msgGpsErr)+strlen(message)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgGpsErr);
	strcat(send,message);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)strlen(send)+1);
	free(send);
	return;
}

void send_compass_error(char * message, int length){
	buildHeader(head);
	char * send = malloc(strlen(head)+strlen(msgCompassErr)+strlen(message)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgCompassErr);
	strcat(send,message);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)strlen(send)+1);
	free(send);
	return;
}

void send_compass_calibration_begin(){
	buildHeader(head);
	char * send = malloc(strlen(head)+strlen(msgCompassCalBegin)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgCompassCalBegin);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void send_compass_calibration_end(){
	buildHeader(head);
	char * send = malloc(strlen(head)+strlen(msgCompassCalEnd)+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgCompassCalEnd);
	send[strlen(send)] = ';';
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

	int sendspeedtime1 = 0;
	int senddirtime1 = 0;

	int sendspeedtime2 = 0;
	int senddirtime2 = 0;
	readdata(bufferedtime,&sendspeed,&senddir);
	//readdata 600
	readdata(timeintervall1,&sendspeedtime1,&senddirtime1);
	//readdata 240
	readdata(timeintervall2,&sendspeedtime2,&senddirtime2);
	buildHeader(head);
	char * speedstr = NULL;
	char * dirstr = NULL;

	char * speedstrtime1 = NULL;
	char * dirstrtime1 = NULL;

	char * speedstrtime2 = NULL;
	char * dirstrtime2 = NULL;

	if(sprintf(speedstr,"%d",sendspeed)<=0){
		//error
	}
	if(sprintf(dirstr,"%d",senddir)<=0){
		//error
	}
	if(sprintf(speedstrtime1,"%d",sendspeedtime1)<=0){
		//error
	}
	if(sprintf(dirstrtime1,"%d",senddirtime1)<=0){
		//error
	}
	if(sprintf(speedstrtime2,"%d",sendspeedtime2)<=0){
		//error
	}
	if(sprintf(dirstrtime2,"%d",senddirtime2)<=0){
		//error
	}
	char * send = malloc(strlen(head)+strlen(deviceID)+strlen(msgDataPacket)
			+strlen(speedstr)+strlen(dirstr)
			+strlen(speedstrtime1)+strlen(dirstrtime1)+strlen(speedstrtime2)
			+strlen(dirstrtime2)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgDataPacket);
	strcat(send,speedstr);
	strcat(send,dirstr);
	strcat(send,speedstrtime1);
	strcat(send,dirstrtime1);
	strcat(send,speedstrtime2);
	strcat(send,dirstrtime2);
	//senden aktuell gemittelt über 900 und funktion mittelwert -x(600)+mittelwert y(240)
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void config_magneticdeclination(int declinationd){
	magneticdeclination = declinationd;
}

void config_databuffers(unsigned int buftime){
	bufferedtime = buftime;
	init_data_buffers();
}

void config_time1(int time1){
	timeintervall1 = time1;
}

void config_time2(int time2){
	timeintervall2 = time2;
}

void config_time(uint32_t hour,uint32_t min,uint32_t sec){
	Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_HOUR,hour);
	Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_MINUTE,min);
	Chip_RTC_SetTime(LPC_RTC,RTC_TIMETYPE_SECOND,sec);
}

void config_updatebasestation(unsigned int rate){
	updaterate = rate;
	init_data_buffers();
}

void config_deviceID(char * id){
	deviceID[0]=id[0];
	deviceID[1]=id[1];
}

void config_updatewinddir(unsigned int adcbuffersize, unsigned int adcsamplerate){
	buffersize = adcbuffersize;
	samplerate = adcsamplerate;
	setupadc();
}

void buildHeader(char * header){

	Chip_RTC_GetFullTime(LPC_RTC, &time);
	char hours[4];
	char mins[4];
	char secs[4];
	if(sprintf(hours,"%d",(int)time.time[RTC_TIMETYPE_HOUR])<=0){
		//error
	}
	if(sprintf(mins,"%d",(int)time.time[RTC_TIMETYPE_MINUTE])<=0){
		//error
	}
	if(sprintf(secs,"%d",(int)time.time[RTC_TIMETYPE_SECOND])<=0){
		//error
	}
	header = malloc(21+strlen(deviceID)+strlen(hours)+strlen(mins)+strlen(secs)+1);
	if (header == NULL){
		send_mem_err();
		free(header);
		return;
	}
	struct gps_data * pos = get_gps_data();
	strcpy(header,deviceID);
	strcat(header,hours);
	strcat(header,mins);
	strcat(header,secs);
	strcat(header, pos->lat);
	strcat(header, pos->north);
	strcat(header, pos->lon);
	strcat(header, pos->east);
}

void send_mem_err(){
	radio_send_variable_packet((uint8_t *)deviceID,(uint16_t)(strlen(deviceID)+1));
	radio_send_variable_packet((uint8_t *)msgMemoryErr,(uint16_t)(strlen(msgMemoryErr)+1));
}

void get_config_message(char * message){
	char decryptedMessage[messagelength];
	decrypt(message, decryptedMessage);
	char * str = strtok(decryptedMessage,":");
	if( str == NULL){
		//fixme error or no match
		return;
	}
	if (strcmp(str,"conf" ) == 0){ //valid config message
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,password ) == 0){ //password check passed
			str = strtok(NULL,":"); //read config word
			if( str == NULL){
				//fixme error or no match
				return;
			}
			parse_config_message(str);

		}

	}

}

void parse_config_message(char * message){
	char * str = strtok(message,":");
	if( str == NULL){
		//fixme error or no match
		return;
	}
	if(strcmp(str,"mgdc") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_value("mgdc",magneticdeclination);
			return;
		}
		int val = atoi(str);
		config_magneticdeclination(val);
	}else if(strcmp(str,"datb") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_value("datb",bufferedtime);
			return;
		}
		int val = atoi(str);
		config_databuffers((unsigned int)val);

	}else if(strcmp(str,"time") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		int hour = atoi(str);
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		int min = atoi(str);
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		int sec = atoi(str);
		config_time((uint32_t)hour,(uint32_t)min,(uint32_t)sec);

	}else if(strcmp(str,"upbs") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_value("upbs",updaterate);
			return;
		}
		int rate = atoi(str);
		config_updatebasestation((unsigned int)rate);
	}else if(strcmp(str,"dvid") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		config_deviceID(str);
	}else if(strcmp(str,"upwd") == 0){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_values("upwd",buffersize,samplerate);
			return;
		}
		int size = atoi(str);
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		int rate = atoi(str);
		config_updatewinddir(size,rate);
	}else if(strcmp(str,"tin1")){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_value("tin1",timeintervall1);
			return;
		}
		int time1 = atoi(str);
		config_time1(time1);
	}else if(strcmp(str,"tin2")){
		str = strtok(NULL,":");
		if( str == NULL){
			//fixme error or no match
			return;
		}
		if(strcmp(str,"?") == 0){
			send_parameter_value("tin2",timeintervall2);
			return;
		}
		int time2 = atoi(str);
		config_time2(time2);
	}
	//......
	else{
		//no valid config symbol
		return;
	}
}

void encrypt(char * message, char * encrypted){
	for(int i = 0; i < messagelength-1; i++){
		encrypted[i] = message[i]  ^ key [i%strlen(key)];
	}
}

void decrypt(char * encrypted, char * message){
	for(int i = 0; i < messagelength-1; i++){
		message[i] = encrypted[i] ^ key [i%strlen(key)];
	}
}

void send_parameter_value(char * parameter, int value ){
	buildHeader(head);
	char * valstr = NULL;
	if(sprintf(valstr,"%d",value)<=0){
		//error
	}

	char * send = malloc(strlen(head)+strlen(msgStatusPacket)
			+strlen(valstr)+strlen(parameter)+1+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgStatusPacket);
	strcat(send,parameter);
	strcat(send,":");
	strcat(send,valstr);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;
}

void send_parameter_values(char * parameter, int value1, int value2){
	buildHeader(head);
	char * valstr1 = NULL;
	if(sprintf(valstr1,"%d",value1)<=0){
		//error
	}
	char * valstr2 = NULL;
	if(sprintf(valstr2,"%d",value2)<=0){
		//error
	}
	char * send = malloc(strlen(head)+strlen(msgStatusPacket)
			+strlen(valstr1)+1+strlen(valstr2)+strlen(parameter)+1+strlen(deviceID)+1);
	if (send == NULL){
		send_mem_err();
		free(send);
		return;
	}
	strcpy(send,deviceID);
	strcat(send,head);
	strcat(send,msgStatusPacket);
	strcat(send,parameter);
	strcat(send,":");
	strcat(send,valstr1);
	strcat(send,":");
	strcat(send,valstr2);
	send[strlen(send)] = ';';
	radio_send_variable_packet((uint8_t *)send,(uint16_t)(strlen(send)+1));
	free(send);
	return;

}
