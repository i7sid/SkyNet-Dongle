/*
 * udp.h
 *
 *  Created on: 15.04.2016
 *      Author: michi
 */

#ifndef IP_UDP_H_
#define IP_UDP_H_

#include "chip.h"

void udp_send(uint8_t dest_ip[4], uint16_t src_port, uint16_t dest_port, uint8_t *data, uint8_t size);




#endif /* IP_UDP_H_ */
