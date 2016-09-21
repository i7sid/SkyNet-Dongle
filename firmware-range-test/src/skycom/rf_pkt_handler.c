/**
 *  @file
 *
 *  @date 	10.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

//#ifndef IS_REPEATER

#include "rf_pkt_handler.h"

uint8_t last_recv_seqno = 0xFF;

void skynet_received_packet(skynet_packet *pkt) {

	uint8_t buf_pre[] = "DONGLE|";
	uint8_t buf_post[16];

	snprintf((char*)buf_post, sizeof(buf_post), "|%d\n", pkt->rssi);

	// send to host
	skynet_cdc_write_buffered(buf_pre, 7);
	skynet_cdc_write_buffered((uint8_t*)pkt->data, pkt->length);
	skynet_cdc_write_buffered(buf_post, strlen((char*)buf_post));

	skynet_led_blink_passive(500);
}
//#endif // !IS_REPEATER
