/*
 * udp.c
 *
 *  Created on: 15.04.2016
 *      Author: michi
 */

#if 0

#include "udp.h"
#include "../cpu/nv_storage.h"
#include "../mac/mac.h"

void udp_send(uint8_t dest_ip[4], uint16_t src_port, uint16_t dest_port, uint8_t *data, uint8_t size) {
	uint16_t cnt = 20 + 8 + size; // TODO
	uint8_t buf[cnt];
	uint16_t id = (uint16_t)mac_random(0x10000);
	NV_DATA_T *conf = skynet_nv_get();

	mac_frame_data frame;
	mac_frame_data_init(&frame);
	frame.payload = buf;
	frame.payload_size = cnt;
	frame.mhr.src_address[0] = conf->mac_addr[0];
	frame.mhr.src_address[1] = conf->mac_addr[1];
	frame.mhr.src_address[2] = conf->mac_addr[2];
	frame.mhr.src_address[3] = conf->mac_addr[3];
	frame.mhr.src_address[4] = conf->mac_addr[4];
	frame.mhr.src_address[5] = conf->mac_addr[5];
	frame.mhr.src_address[6] = 0x00;
	frame.mhr.src_address[7] = 0x08;
	// TODO: dest MAC ADDRESS?

	MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_LONG);
	MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_LONG);

	// construct IP packet
	uint16_t i = 0;
	buf[i++] = 0x45;			// version: 4, header length: 20
	buf[i++] = 0x0;
	buf[i++] = ((cnt && 0xFF00) >> 8);	// total length
	buf[i++] = ((cnt && 0x00FF)     );	// total length
	buf[i++] = ((id && 0xFF00) >> 8);	// identification
	buf[i++] = ((id && 0x00FF)     );	// identification
	buf[i++] = 0x40;			// flag: don't fragment
	buf[i++] = 0x0;				// offset: 0
	buf[i++] = 0x40;			// TTL: 64
	buf[i++] = 0x11;			// proto: UDP
	buf[i++] = 0x0;			// header checksum
	buf[i++] = 0x0;			// header checksum
	buf[i++] = conf->ipv4_addr[0];		// src ip
	buf[i++] = conf->ipv4_addr[1];		// src ip
	buf[i++] = conf->ipv4_addr[2];		// src ip
	buf[i++] = conf->ipv4_addr[3];		// src ip
	buf[i++] = dest_ip[0];				// dest ip
	buf[i++] = dest_ip[1];				// dest ip
	buf[i++] = dest_ip[2];				// dest ip
	buf[i++] = dest_ip[3];				// dest ip

	// udp header
	buf[i++] = ((src_port && 0xFF00) >> 8);		// src port
	buf[i++] = ((src_port && 0x00FF)     );		// src port
	buf[i++] = ((dest_port && 0xFF00) >> 8);	// dest port
	buf[i++] = ((dest_port && 0x00FF)     );	// dest port
	buf[i++] = ((size && 0xFF00) >> 8);			// length
	buf[i++] = ((size && 0x00FF)     );			// length
	buf[i++] = 0x0;								// checksum
	buf[i++] = 0x0;								// checksum

	memcpy(buf+i, data, size);

	mac_transmit_packet(&frame, true);
}
#endif
