/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <fstream>
#include <iomanip>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <netinet/ip.h>

#include <usb/message.h>
#include <mac/mac.h>
#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"
#include "cmdline.h"
#include "gui/gui.h"


using namespace std;

extern tap* ptr_tap;
extern usb_tty* ptr_tty;
extern cmdline arg_parser;



void usbReceiveHandler(usb_message pkt) {
	if (pkt.type == USB_DEBUG) {
        COLOR_DBG();
        for (unsigned int i = 0; i < pkt.payload_length; ++i) {
            cerr << (char)pkt.payload[i];
        }
        COLOR_RESET();
	}
	if (arg_parser.get().verbosity >= 3) {
		cout << "Received USB message: " << endl;
		cout << "Type:\t" << (unsigned int)pkt.type << endl;
		cout << "SeqNo:\t" << (unsigned int)pkt.seqno << endl;
		cout << "Length:\t" << pkt.payload_length << endl;
		cout << "Payload:" << endl;

		for (unsigned int i = 0; i < pkt.payload_length; ++i) {
			cout << setfill(' ') << setw(3) << std::hex << "0x" <<
                ((unsigned int)pkt.payload[i] & 0xFF) << " " << std::dec;
		}
		cout << endl;
		for (unsigned int i = 0; i < pkt.payload_length; ++i) {
			cout <<  " " << (char)pkt.payload[i] << "  ";
		}
		cout << endl << endl;

	}

	if (pkt.type == USB_SKYNET_PACKET) {
		mac_frame_data frame;
        pkt.payload_length -= PKT_DBG_OVERHEAD;
		mac_frame_data_unpack(&frame, (uint8_t*)pkt.payload, (unsigned int)pkt.payload_length);

		// construct ethernet frame and write to tap device
		char ether_frame[sizeof(struct ether_header) + frame.payload_size];
		memset(ether_frame, 0, sizeof(ether_frame));

		struct ether_header* ether_hdr = (struct ether_header*)(ether_frame);
		char* data = (char*)(ether_frame + sizeof(struct ether_header));

		memset(ether_hdr->ether_dhost, 0, sizeof(ether_hdr->ether_dhost));
		memset(ether_hdr->ether_shost, 0, sizeof(ether_hdr->ether_shost));

		if (MHR_FC_GET_DEST_ADDR_MODE(frame.mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
			ether_hdr->ether_dhost[0] = frame.mhr.dest_address[0];
			ether_hdr->ether_dhost[1] = frame.mhr.dest_address[1];
		}
		else if (MHR_FC_GET_DEST_ADDR_MODE(frame.mhr.frame_control) == MAC_ADDR_MODE_LONG) {
			ether_hdr->ether_dhost[0] = frame.mhr.dest_address[0];
			ether_hdr->ether_dhost[1] = frame.mhr.dest_address[1];
			ether_hdr->ether_dhost[2] = frame.mhr.dest_address[2];
			ether_hdr->ether_dhost[3] = frame.mhr.dest_address[3];
			ether_hdr->ether_dhost[4] = frame.mhr.dest_address[4];
			ether_hdr->ether_dhost[5] = frame.mhr.dest_address[5];
		}

		if (MHR_FC_GET_DEST_ADDR_MODE(frame.mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
			ether_hdr->ether_shost[0] = frame.mhr.src_address[0];
			ether_hdr->ether_shost[1] = frame.mhr.src_address[1];
		}
		else if (MHR_FC_GET_DEST_ADDR_MODE(frame.mhr.frame_control) == MAC_ADDR_MODE_LONG) {

			ether_hdr->ether_shost[0] = frame.mhr.src_address[0];
			ether_hdr->ether_shost[1] = frame.mhr.src_address[1];
			ether_hdr->ether_shost[2] = frame.mhr.src_address[2];
			ether_hdr->ether_shost[3] = frame.mhr.src_address[3];
			ether_hdr->ether_shost[4] = frame.mhr.src_address[4];
			ether_hdr->ether_shost[5] = frame.mhr.src_address[5];
		}


		mac_payload_type frame_type = mac_payload_type::NONE;

		// process extheaders
        ether_hdr->ether_type = 0;
        mac_extheader* next_hdr = frame.extheader;
        while (next_hdr != NULL) {
        	switch (next_hdr->typelength_union.type_length.type) {
				case mac_extheader_types::EXTHDR_ETHER_TYPE:
                    frame_type = mac_payload_type::ETHERFRAME;
					ether_hdr->ether_type = ((uint16_t)(next_hdr->data[0]) << 8)
							                           + next_hdr->data[1];
					break;

				case mac_extheader_types::EXTHDR_SENSOR_VALUES:
					// TODO Sensordaten verarbeiten (in DB einfügen)
					//char sensorbuf[128];
					frame_type = mac_payload_type::BASE_SENSOR_DATA;
					cerr << frame.payload << endl;
					break;

				case mac_extheader_types::EXTHDR_DONGLE_CMD_ANSWER:
                    {
                        uint8_t seqno = next_hdr->data[0];
                        uint8_t ack = next_hdr->data[1];

                        if (ack == 0) {
                            COLOR_OK();
                            cout << "Dongle command " << (((unsigned int)seqno) & 0xFF)
                                << ": ACK." << endl;
                            COLOR_RESET();
                        }
                        else {
                            COLOR_ERR();
                            cout << "Dongle command " << (((unsigned int)seqno) & 0xFF)
                                << ": Error (" << (((unsigned int)ack) & 0xFF)
                                << ")." << endl;
                            COLOR_RESET();
                        }
                        break;
                    }

                default:
                    break;
        	}
        	next_hdr = next_hdr->next;
        }


        if (arg_parser.get().use_tap) {
			// send ethernet frame
			if (frame_type == mac_payload_type::ETHERFRAME) {
				memcpy(data, frame.payload, frame.payload_size);
				ptr_tap->send_packet(ether_frame, sizeof(ether_frame));
			}
        }
		// cleanup mac packet
		free(frame.payload);
		mac_frame_extheaders_free(frame.extheader);
	}

	// cleanup usb packet
	delete[] pkt.payload;
}



