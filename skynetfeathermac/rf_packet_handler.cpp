/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-08-31
 */

#include "rf_packet_handler.h"

#define skycom_send_donglecmd_answer(a,b,c) {}

uint8_t last_recv_seqno = 0xFF;

extern skynetfeatherconfig cfg;


void rf_handler(uint8_t* data, uint8_t length) {
    bool to_usb = true;
    bool my_mac = false;
    uint8_t ttl = 0;

    mac_frame_data inframe;
    mac_frame_data_init(&inframe);

    mac_frame_data_unpack(&inframe, data, length);

    if (MHR_FC_GET_DEST_ADDR_MODE(inframe.mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
        if (inframe.mhr.dest_address[0] == cfg.mac0 && // me
                inframe.mhr.dest_address[1] == cfg.mac1) {

            to_usb = false;
            my_mac = true;
        }
        else if (inframe.mhr.dest_address[0] == 0xFF &&                 // broadcast
                inframe.mhr.dest_address[1] == 0xFF) {

            to_usb = false;
            my_mac = false;
        }
    }
    /*
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
    */

    // TODO erkennen, ob das Paket schon mal empfangen wurde (verlorenes ACK)
    // TODO erkennen, dass Paket für angeschlossenes USB-Gerät gedacht ist

    if (last_recv_seqno != inframe.mhr.seq_no) {
    //if (!packet_seen(inframe.mhr.src_address[0], inframe.mhr.src_address[1], inframe.mhr.seq_no)) {
        last_recv_seqno = inframe.mhr.seq_no;

        // seems to be for me
        if (!to_usb) {
            to_usb = true; // first reset

            mac_extheader* next_hdr = inframe.extheader;
            while (next_hdr != NULL) {
                //Serial.println((uint32_t)(next_hdr->next), HEX);
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
                            frame.mhr.dest_address[0] = inframe.mhr.src_address[0];
                            frame.mhr.dest_address[1] = inframe.mhr.src_address[1];
                            frame.mhr.src_address[0] = cfg.mac0;
                            frame.mhr.src_address[1] = cfg.mac1;

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
                            cfg.radio_pa_level = inframe.payload[0];

                            // send answer
                            skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);

                            // set new parameters
                            //vRadio_Change_PwrLvl(config->radio_pa_level); // also goes to READY state! (leaves RX)
                            //vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, 0x0);
                            // TODO

                            // send answer again with new parameters
                            delay(250);
                            skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);

                        }
    #ifdef IS_BASESTATION
                        else if (next_hdr->data[0] == CALIB_COMPASS) {
                            // TODO: Wie?

                            // send answer
                            skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
                        }
                        else if (next_hdr->data[0] == CALIB_COMPASS_STOP) {
                            // TODO: Wie?
                            
                            // send answer
                            skycom_send_donglecmd_answer(inframe.mhr.src_address, inframe.mhr.seq_no, 0);
                        }
                        /*
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
                        */
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

        /*
        // ACK packet if needed
        if (my_mac) {
            if (MHR_FC_GET_ACK_REQUEST(inframe.mhr.frame_control)) {
                mac_transmit_ack(inframe.mhr.seq_no);
            }
        }
        */
    }


    // Give this packet to USB.
    if (to_usb) {
        // send to host
        usb_message msg;
        msg.magic = 43981;
        msg.seqno = 0;
        msg.type = USB_SKYNET_PACKET;
        msg.payload_length = length;

        msg.payload = (char*)data;
        //char buf[length];
        //msg.payload = buf;
        //memcpy(buf, data, length);

        Serial.write((char*)&msg, USB_HEADER_SIZE);
        Serial.write((char*)(msg.payload), msg.payload_length);
    }


    // check if we have to resend this frame (basic multihop)
    if (ttl > 0 && !my_mac) {   // rehop!
        // transmit packet again with new ttl

        mac_frame_data frame;
        memcpy(&(frame.mhr), &(inframe.mhr), sizeof(frame.mhr));
        frame.payload = inframe.payload;
        frame.payload_size = inframe.payload_size;

        frame.extheader = inframe.extheader; // ttl was decreased above

        // wait for a moment to reduce collisions
        delay(50);

        // resend frame
        mac_transmit_packet(&frame, false);
    }

    // Must be done! Memory was allocated dynamically.
    //mac_frame_extheaders_free(inframe.extheader);
    mac_frame_data_free_contents(&inframe);
}




