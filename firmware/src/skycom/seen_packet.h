/*
 * seen_packet.h
 *
 *  Created on: 
 *      Author: michi
 */
/**
 *  @file
 *
 *  @date 	24.06.2016
 *  @author	michi
 */

#ifndef SKYCOM_SEEN_PACKET_H_
#define SKYCOM_SEEN_PACKET_H_

typedef struct seen_packet {
	uint8_t src_0;
	uint8_t src_1;
	uint8_t seqno;
} seen_packet;

#endif /* SKYCOM_SEEN_PACKET_H_ */
