/** @file
 *
 *
 *  @date 06.06.2015
 *  @author Christoph Kohl
 *
 *  @brief Functions for the UBX/NMEA-Protocol
 */

#include "ubx_nmea_parser.h"
#include "../../misc/debug.h"
#include <string.h>

bool checksum(char * message){
	int count = 3;
	uint8_t ch_a = 0;
	uint8_t ch_b = 0;
	while(message[count] != '*'){
		ch_a = ch_a + message[count];
		ch_b = ch_a +ch_b;
		count++;
		//DBG("%d %c \n",count,message[count]);
	}

	return ((ch_a == message[count +1])&&(ch_b == message[count +2]));
}

bool checkAck(char * message){
	char * ack = {'0xb5','0x62','0x05','0x01'};
	if(strncmp(message,ack,4)==0){
			if(message[3] == '0x01'){
				return true;
			}
			return false;
		}else{
			return false;
		}
}

