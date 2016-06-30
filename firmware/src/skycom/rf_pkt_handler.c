/**
 *  @file
 *
 *  @date 	10.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

//#ifndef IS_REPEATER

#include "rf_pkt_handler.h"
#include "seen_packet.h"


uint8_t last_recv_seqno = 0xFF;

void skynet_received_packet(skynet_packet *pkt) {
	//DBG("pkt recv (length: %d)\n", pkt->length);

	/*
	for (int i = pkt->length - 8; i < pkt->length; ++i) {
		DBG("0x%x ", (pkt->data[i] & 0xFF));
	}
	DBG("\n");
	*/

	NV_DATA_T *config = skynet_nv_get();

	bool to_usb = true;
	bool my_mac = false;
	uint8_t ttl = 0;

	mac_frame_data inframe;
	mac_frame_data_init(&inframe);

	mac_frame_data_unpack(&inframe, (uint8_t*)pkt->data, pkt->length);

	if (MHR_FC_GET_DEST_ADDR_MODE(inframe.mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
		if ((inframe.mhr.dest_address[0] == config->mac_addr[4] &&	// me
				inframe.mhr.dest_address[1] == config->mac_addr[5]) ||
			(inframe.mhr.dest_address[0] == 0xFF &&					// broadcast
				inframe.mhr.dest_address[1] == 0xFF)) {

			to_usb = false;
		}
	}
	else if (MHR_FC_GET_DEST_ADDR_MODE(inframe.mhr.frame_control) == MAC_ADDR_MODE_LONG) {
		if (inframe.mhr.dest_address[0] == config->mac_addr[0] &&
				inframe.mhr.dest_address[1] == config->mac_addr[1] &&
				inframe.mhr.dest_address[2] == config->mac_addr[2] &&
				inframe.mhr.dest_address[3] == config->mac_addr[3] &&
				inframe.mhr.dest_address[4] == config->mac_addr[4] &&
				inframe.mhr.dest_address[5] == config->mac_addr[5]) {

			to_usb = false;
			my_mac = true;
		}
	}

	// TODO erkennen, ob das Paket schon mal empfangen wurde (verlorenes ACK)
	// TODO erkennen, dass Paket für angeschlossenes USB-Gerät gedacht ist

	//if (last_recv_seqno != inframe.mhr.seq_no) {
	if (!packet_seen(inframe.mhr.src_address[0], inframe.mhr.src_address[1], inframe.mhr.seq_no)) {
		last_recv_seqno = inframe.mhr.seq_no;

		// seems to be for me
		if (!to_usb) {
			to_usb = true; // first reset

			mac_extheader* next_hdr = inframe.extheader;
			while (next_hdr != NULL) {
				switch (next_hdr->typelength_union.type_length.type) {
					case EXTHDR_DONGLE_CMD:
					{
						// really for me, so don't send to usb later
						to_usb = false;

						if (next_hdr->data[0] == TEST) {
							// send answer

							mac_frame_data frame;
							mac_frame_data_init(&frame);
							frame.payload = NULL;
							frame.payload_size = 0;

							MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
							MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);

							// MAC addresses
							NV_DATA_T *config = skynet_nv_get();
							frame.mhr.dest_address[0] = inframe.mhr.src_address[0];
							frame.mhr.dest_address[1] = inframe.mhr.src_address[1];
							frame.mhr.src_address[0] = config->mac_addr[4];
							frame.mhr.src_address[1] = config->mac_addr[5];

							// ext headers
							mac_extheader hdr;
							mac_extheader_init(&hdr);
							hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD_ANSWER;
							hdr.typelength_union.type_length.length = 2;
							hdr.data[0] = inframe.mhr.seq_no;
							hdr.data[1] = 0;

							frame.extheader = &hdr;


							// send frame
							mac_transmit_packet(&frame, true);

						}
						else if (next_hdr->data[0] == RF_SET_PARAMETERS) {
							if (inframe.payload_size < 1) break;
							NV_DATA_T* config = skynet_nv_get();
							config->radio_pa_level = inframe.payload[0];
							skynet_nv_write(config);

							// send answer
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);

							// set new parameters
							vRadio_Change_PwrLvl(config->radio_pa_level); // also goes to READY state! (leaves RX)
							vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, 0x0);

							// send answer again with new parameters
							msDelayActive(250);
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);

						}
	#ifdef IS_BASESTATION
						else if (next_hdr->data[0] == CALIB_COMPASS) {
							// now start calibration
							skynetbase_compass_start_calibration();

							// send answer
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
						}
						else if (next_hdr->data[0] == CALIB_COMPASS_STOP) {
							// now stop calibration and save data
							skynetbase_compass_stop_calibration();
							NV_DATA_T* config = skynet_nv_get();
							memcpy(&(config->compass_calibration), skynetbase_compass_calibration_get(), sizeof(compass_calibration_data));
							skynet_nv_write(config); // now save to flash

							// send answer
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
						}
						else if (next_hdr->data[0] == BASE_START_SENDING) {
							uint32_t wind_wait = 0;
							uint32_t pos_wait = 0;
							if (inframe.payload_size < 8) {
								// TODO error
								break;
							}
							wind_wait += inframe.payload[0];
							wind_wait += inframe.payload[1] << 8;
							wind_wait += inframe.payload[2] << 16;
							wind_wait += inframe.payload[3] << 24;
							pos_wait += inframe.payload[4];
							pos_wait += inframe.payload[5] << 8;
							pos_wait += inframe.payload[6] << 16;
							pos_wait += inframe.payload[7] << 24;
							v_configuration.time_wind_wait = wind_wait;
							v_configuration.time_pos_wait = pos_wait;

							// send regularily data events
							remove_delayed_event(generate_event_wind_base);
							remove_delayed_event(generate_event_pos_base);
							register_delayed_event(v_configuration.time_wind_wait, generate_event_wind_base);
							register_delayed_event(v_configuration.time_pos_wait, generate_event_pos_base);

							// send answer
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
						}
						else if (next_hdr->data[0] == BASE_STOP_SENDING) {
							v_configuration.time_pos_wait = 0;
							v_configuration.time_wind_wait = 0;

							remove_delayed_event(generate_event_wind_base);
							remove_delayed_event(generate_event_pos_base);

							// send answer
							skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
						}
	#endif
						break;
					}
					case EXTHDR_TTL:
					{
						if (next_hdr->typelength_union.type_length.length == 1) {
							ttl = next_hdr->data[0];
							ttl -= 1;
							next_hdr->data[0] = ttl;
						}
						break;
					}
					default:
						break;
				}
				next_hdr = next_hdr->next;
			}
		}

		// ACK packet if needed
		if (my_mac) {
			if (MHR_FC_GET_ACK_REQUEST(inframe.mhr.frame_control)) {
				mac_transmit_ack(inframe.mhr.seq_no);
			}
		}
	}

	// Damn, that wasn't for me...
	if (to_usb) {
		// send to host
		usb_message msg;
		msg.seqno = 0; 							// chose automatically next one
		msg.type = USB_SKYNET_PACKET;
		msg.payload_length = pkt->length;
		char buf[pkt->length];
		msg.payload = buf;
		memcpy(buf, pkt->data, pkt->length);

		skynet_cdc_write_message(&msg);

	}

#ifdef IS_REPEATER
	// must be not for me
	if (!my_mac) {
		if (ttl > 0) {
			mac_frame_data frame;
			memcpy(&(frame.mhr), &(inframe.mhr), sizeof(frame.mhr));
			frame.payload = inframe.payload;
			frame.payload_size = inframe.payload_size;

			frame.extheader = inframe.extheader; // ttl was decreased above

			// send frame
			mac_transmit_packet(&frame, false);
		}
	}
#endif

	// Must be done! Memory was allocated dynamically.
	//mac_frame_extheaders_free(inframe.extheader);
	mac_frame_data_free_contents(&inframe);
	free(pkt->data);	free_count();
	free(pkt);			free_count();

	skynet_led_blink_passive(5);
}
//#endif // !IS_REPEATER
