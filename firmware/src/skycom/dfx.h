/**
 * dfx is short for "dongle feather exchange" and enables the dongle to communicate with an adafruit feather.
 *
 *  @file
 *
 *  @date 	29.07.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SKYCOM_DFX_H_
#define SKYCOM_DFX_H_

#include "chip.h"

#define		DFX_RX_PORT		0
#define		DFX_RX_PIN 		9
#define		DFX_TX_PORT		0
#define		DFX_TX_PIN 		8


typedef struct dfx_packet {
	uint8_t			magic;
	uint8_t			time[6];
	uint8_t			pos[32];
	float			wind_speed;
	uint16_t		wind_dir;
	uint16_t		compass;
	uint8_t			checksum;
} __attribute__((aligned(1),packed)) dfx_packet;


void dfx_init(void);

void dfx_packet_init(dfx_packet*);
void dfx_pack_and_send(void);

void dfx_set_time(uint8_t*);
void dfx_set_pos(uint8_t*);
void dfx_set_pos_n(uint8_t*, uint16_t);
void dfx_set_wind_speed(float);
void dfx_set_wind_dir(uint16_t);
void dfx_set_compass(uint16_t);

inline uint8_t dfx_checksum_calc(dfx_packet* pkt) {
    uint8_t sum = 0;
    uint8_t* pos = (uint8_t*)pkt;
    uint32_t n = sizeof(dfx_packet);

    while (n-- > 0) { sum += *(pos++); }

    return (~sum);
}

#endif /* SKYCOM_DFX_H_ */
