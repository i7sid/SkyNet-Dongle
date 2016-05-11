/**
 *  @file
 *
 *  @date 	10.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "skycom.h"

void generate_event_wind_base(void) {
	events_enqueue(EVENT_BASE_SEND_WIND, NULL);
	register_delayed_event(v_configuration.time_wind_wait, generate_event_wind_base);
}

void generate_event_pos_base(void) {
	events_enqueue(EVENT_BASE_QUERY_POS, NULL);
	register_delayed_event(v_configuration.time_pos_wait, generate_event_pos_base);
}


void skycom_send_donglecmd_answer(uint8_t dest[], uint8_t seq_no, uint8_t reply) {
	mac_frame_data frame;
	mac_frame_data_init(&frame);
	frame.payload = NULL;
	frame.payload_size = 0;

	MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
	MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);

	// MAC addresses
	NV_DATA_T *config = skynet_nv_get();
	frame.mhr.dest_address[0] = dest[0];
	frame.mhr.dest_address[1] = dest[1];
	frame.mhr.src_address[0] = config->mac_addr[4];
	frame.mhr.src_address[1] = config->mac_addr[5];

	// ext headers
	mac_extheader hdr;
	mac_extheader_init(&hdr);
	hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD_ANSWER;
	hdr.typelength_union.type_length.length = 2;
	hdr.data[0] = seq_no;
	hdr.data[1] = reply;

	frame.extheader = &hdr;

	// send frame
	mac_transmit_packet(&frame);
}

