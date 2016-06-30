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

#include "chip.h"

typedef struct seen_packet_src {
	uint8_t src_0;
	uint8_t src_1;
} __attribute__((aligned(1),packed)) seen_packet_src;

typedef struct seen_packet {
	seen_packet_src	src;
	uint8_t			seqno;
} __attribute__((aligned(1),packed)) seen_packet;


bool packet_seen(uint8_t src0, uint8_t src1, uint8_t seqno);

#endif /* SKYCOM_SEEN_PACKET_H_ */
