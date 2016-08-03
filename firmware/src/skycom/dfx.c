/**
 *  @file
 *
 *  @date 	29.07.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <string.h>
#include <../misc/misc.h>

#include "dfx.h"
#include "LPC_SWU.h"
#include "../cpu/systick.h"

static dfx_packet current_data;

void dfx_init(void) {
	dfx_packet_init(&current_data);

	// init pins
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	// debug
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	/*
	// test which pin is which
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
	*/


	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, DFX_TX_PORT, DFX_TX_PIN, IOCON_FUNC3);
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER2);
	//Chip_TIMER_Init(LPC_TIMER2);


	swu_init(LPC_TIMER2);

	/*
	while(1) {
		Chip_GPIO_SetPinOutLow(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
		msDelayActive(50);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
		msDelayActive(50);
	}
	*/


	/*
	unsigned char i = 0;
	while(1) {
		swu_tx_chr(i);
		i++;
		if (i > 127) i = 0;
		msDelayActive(1000);
	}
	*/


}

void dfx_set_time(uint8_t* t) {
	uint8_t n = MIN(sizeof(current_data.time), strlen((char*)t));
	memcpy(current_data.time, t, n);
}

void dfx_set_pos(uint8_t* p) {
	uint8_t n = MIN(sizeof(current_data.time), strlen((char*)p));
	memcpy(current_data.pos, p, n);
}

void dfx_set_pos_n(uint8_t* p, uint16_t n) {
	uint8_t nn = MIN(sizeof(current_data.time), n);
	memcpy(current_data.pos, p, nn);
}

void dfx_set_wind_speed(float s) {
	current_data.wind_speed = s;
}

void dfx_set_wind_dir(uint16_t d) {
	current_data.wind_dir = d;
}

void dfx_set_compass(uint16_t c) {
	current_data.compass = c;
}

void dfx_packet_init(dfx_packet* pkt) {
	memset(pkt, 0, sizeof(dfx_packet));
	pkt->magic = 0xAA;
}

void dfx_pack_and_send(void) {
	current_data.checksum = 0;
	current_data.checksum = dfx_checksum_calc(&current_data);

	for (uint16_t i = 0; i < sizeof(dfx_packet); ++i) {
		swu_tx_chr(((uint8_t*)&current_data)[i]);
	}
}
