/**
 *  @file
 *
 *  @date 	30.06.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "seen_packet.h"

#define REMEMBER_STATIONS	(8)

// as in BSS this is initialized as 0
static seen_packet last_packets[REMEMBER_STATIONS];

bool packet_seen(uint8_t src0, uint8_t src1, uint8_t seqno) {
	for (uint8_t i = 0; i < REMEMBER_STATIONS; ++i) {
		if (last_packets[i].src.src_0 == src0 && last_packets[i].src.src_1 == src1) {
			// already in list
			if (last_packets[i].seqno == seqno) {
				return true;
			}
			else {
				last_packets[i].seqno = seqno;
				return false;
			}
		}
	}

	// not in list
	for (uint8_t i = 0; i < sizeof(last_packets); ++i) {
		if (last_packets[i].src.src_0 == 0 && last_packets[i].src.src_1 == 0) {
			// free entry found
			last_packets[i].src.src_0 = src0;
			last_packets[i].src.src_1 = src1;
			last_packets[i].seqno = seqno;
			return false;
		}
	}

	// no free entry, return false for sure
	return false;
}
