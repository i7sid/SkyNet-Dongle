/*
 * mac_extheader.h
 *
 *  Created on: 
 *      Author: michi
 */
/**
 *  @file
 *
 *  @date 	28.04.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef MAC_MAC_EXTHEADER_H_
#define MAC_MAC_EXTHEADER_H_

#include <string.h>

typedef enum mac_extheader_types {
	EXTHDR_NO			= 0, 	///<@brief No header following.
	EXTHDR_PAYLOAD_TYPE	= 1 	///<@brief Payload type (see \ref mac_payload_type ) following.
} mac_extheader_types;

//typedef struct mac_extheader;

union typelength_union {
	uint8_t raw;
	struct type_length {
		mac_extheader_types		type	: 4;
		uint8_t					length	: 4;
	} type_length;
};

typedef struct mac_extheader {
	union typelength_union  typelength_union;
	uint8_t					data[16];
	struct mac_extheader*	next;
} __attribute__((aligned(1),packed)) mac_extheader;


inline void mac_extheader_init(mac_extheader* hdr) {
	memset(hdr, 0, sizeof(mac_extheader));
}


#endif /* MAC_MAC_EXTHEADER_H_ */
