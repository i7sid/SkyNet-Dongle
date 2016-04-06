/**
 * @file
 *
 * @date	15.03.2016
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifdef CORE_M3
#include <chip.h>
#else
#include "linux_incs.h"
#endif


#include "mac_frame_data.h"
#include "mac.h"

void* mac_frame_data_calloc(void) {
	void *frame = calloc(sizeof(mac_frame_data), 1);
	mac_frame_data_init(frame);
}


void mac_frame_data_free(mac_frame_data *frame) {
	free(frame->payload);
	free(frame);
}


/*
size_t mac_frame_data_get_size(mac_frame_data *frame) {
	// TODO anpassen, siehe unten (mac_frame_data_pack())
    return sizeof(frame->mhr) + frame->payload_size + sizeof(frame->fcs);
}
*/
size_t mac_frame_data_estimate_size(mac_frame_data *frame) {
	// maximum of header fields as defined in max_frame_data + payload - field "payload_size"
    return sizeof(mac_frame_data) + frame->payload_size - sizeof(uint16_t);
}

void mac_frame_data_init(mac_frame_data *frame) {
	memset(frame, 0, sizeof(mac_frame_data));
	MHR_FC_SET_FRAME_TYPE(frame->mhr.frame_control, MAC_FRAME_DATA);
	MHR_FC_SET_FRAME_VERSION(frame->mhr.frame_control, 0x01);

	// TODO set new seq_no
}

uint16_t mac_frame_data_pack(mac_frame_data *frame, uint8_t *buffer) {
	uint16_t pos = 0;
    // copy MHR
	switch ((mac_frame_types)(MHR_FC_GET_FRAME_TYPE(frame->mhr.frame_control))) {
		case MAC_FRAME_BEACON:
			break;
		case MAC_FRAME_DATA:
			// ignore security for first implementation
			MHR_FC_SET_SECURITY_ENABLED(frame->mhr.frame_control, 0);

			// ignore frame pending for first implementation
			MHR_FC_SET_FRAME_PENDING(frame->mhr.frame_control, 0);

			// ignore ack request for first implementation
			MHR_FC_SET_ACK_REQUEST(frame->mhr.frame_control, 0);


			// copy frame control
			buffer[pos++] = frame->mhr.frame_control[0];
			buffer[pos++] = frame->mhr.frame_control[1];

			// copy seqno
			buffer[pos++] = frame->mhr.seq_no;

			// copy dest pan id (if it shall be present)
			if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT) {
				buffer[pos++] = frame->mhr.dest_pan_id[0]; // TODO endianness?
				buffer[pos++] = frame->mhr.dest_pan_id[1];
			}

			// copy dest address
			if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
				// TODO copy 2 bytes (short address)
				pos += 2;
			}
			else if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
				// TODO copy 8 bytes (long address)
				pos += 8;
			}

			if (!(MHR_FC_GET_PAN_ID_COMPRESSION(frame->mhr.frame_control)) &&
					(MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT)) {
				buffer[pos++] = frame->mhr.src_pan_id[0]; // TODO endianess?
				buffer[pos++] = frame->mhr.src_pan_id[1];
			}

			// copy src address
			if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
				// TODO copy 2 bytes (short address)
				pos += 2;
			}
			else if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
				// TODO copy 8 bytes (long address)
				pos += 8;
			}

			// ignore aux security header for now

			break;
		case MAC_FRAME_ACK:
			break;
		case MAC_FRAME_MAC_CMD:
			break;
	}


    // copy payload
    memcpy(buffer + pos, frame->payload, frame->payload_size);
    pos += frame->payload_size;


    // copy emptyfcs
    buffer[pos++] = 0;
    buffer[pos++] = 0;
    //memcpy(buffer + sizeof(frame->mhr) + frame->payload_size, frame->fcs,
    //        sizeof(frame->fcs));

    return pos;
}
