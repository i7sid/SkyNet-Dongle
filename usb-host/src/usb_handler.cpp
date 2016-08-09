/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <map>

#include <usb/message.h>
#include <mac/mac.h>
#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"
#include "cmdline.h"
#include "gui/gui.h"
#include "db/db.h"
#include "station.h"
#include "output/output.h"
#include "string_helper.h"

using namespace std;

extern usb_tty* ptr_tty;
extern cmdline arg_parser;
extern gui gui;

#ifndef NO_TAP
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
extern tap* ptr_tap;
#endif // NO_TAP


map<string, uint8_t> last_seqno;


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
		cout << "Type:\t" << (unsigned int)(pkt.type & 0xFF) << endl;
		cout << "SeqNo:\t" << (unsigned int)(pkt.seqno & 0xFF) << endl;
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

	if (pkt.type == USB_RANGE_TEST) {
        ofstream of;
        of.open("rangetest-rx.csv", std::ofstream::out | std::ofstream::app);

        of << pkt.payload << endl;

        of.close();
    }
    else if (pkt.type == USB_SKYNET_PACKET) {
		mac_frame_data frame;
        pkt.payload_length -= PKT_DBG_OVERHEAD;
		mac_frame_data_unpack(&frame, (uint8_t*)pkt.payload, (unsigned int)pkt.payload_length);


#ifndef NO_TAP
		mac_payload_type frame_type = mac_payload_type::NONE;
        uint16_t ether_type = 0;
#endif // NO_TAP

        // build mac address string
        stringstream mac_builder;
        mac_builder << setfill('0') << setw(2) << std::hex <<
                (unsigned int)(frame.mhr.src_address[0]) << ":" <<
                setfill('0') << setw(2) << std::hex <<
                (unsigned int)(frame.mhr.src_address[1]);

        station &from_s = get_station(mac_builder.str());

		// TODO filter out packets that were received multiple times
		if (last_seqno.count(mac_builder.str()) > 0 && last_seqno[mac_builder.str()] == frame.mhr.seq_no) {
			// packet was seen before, so clean up and abort
			free(frame.payload);
			mac_frame_extheaders_free(frame.extheader);
			return;
		}
		else {
			last_seqno[mac_builder.str()] = frame.mhr.seq_no;
		}

		// process extheaders
        mac_extheader* next_hdr = frame.extheader;
        while (next_hdr != NULL) {
        	switch (next_hdr->typelength_union.type_length.type) {
				case mac_extheader_types::EXTHDR_ETHER_TYPE:
#ifndef NO_TAP
                    frame_type = mac_payload_type::ETHERFRAME;
					ether_type = ((uint16_t)(next_hdr->data[0]) << 8)
							                           + next_hdr->data[1];
#endif // NO_TAP
					break;

				case mac_extheader_types::EXTHDR_SENSOR_VALUES:
				{
					// parse sensor data and add to database

					vector<string> parts = split((const char*)frame.payload, '|');
					if (parts.size() < (size_t)(next_hdr->typelength_union.type_length.length) + 1) {
						COLOR_ERR();
						cerr << endl << "Received invalid sensor data packet. (too few data elements)" << endl;
						cerr << frame.payload << endl;
						COLOR_RESET();
						break;
					}

					time_t t = time(0);   // get time now
					struct tm * now = localtime(&t);
					stringstream db_timestamp;
					db_timestamp << setfill('0') << setw(4)
							<< now->tm_year + 1900 << "-" << setfill('0') << setw(2) << now->tm_mon + 1
							<< "-" << setfill('0') << setw(2) << now->tm_mday
							<< " " << parts[0][0] << parts[0][1] << ":"
							<< parts[0][2] << parts[0][3] << ":"
							<< parts[0][4] << parts[0][5];

					for (int i = 0; i < next_hdr->typelength_union.type_length.length; ++i) {
						if (next_hdr->data[i] == SENSOR_POSITION) {
                            from_s.update_position(parts[i+1]);
                            from_s.set_last_pos_time(db_timestamp.str());
							cerr << "#";
						}
						else if (next_hdr->data[i] == SENSOR_COMPASS) {
                            try {
                                from_s.set_compass(stof(parts[i+1]));
                                from_s.set_last_pos_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_DATE) {
							cerr << "Date: " << parts[i+1] << endl;
						}
						else if (next_hdr->data[i] == SENSOR_WIND_SPEED) {
                            try {
                                from_s.set_wind_speed(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_WIND_DIR) {
                            try {
                                from_s.set_wind_direction(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_WIND_DIR_RAW) {
                            try {
                                from_s.set_wind_direction_raw(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_HIST_WIND_SPEED_SHORT) {
                            try {
                                from_s.set_base_hist_speed_short(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_HIST_WIND_DIR_SHORT) {
                            try {
                                from_s.set_base_hist_dir_short(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_HIST_WIND_SPEED_LONG) {
                            try {
                                from_s.set_base_hist_speed_long(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
						else if (next_hdr->data[i] == SENSOR_HIST_WIND_DIR_LONG) {
                            try {
                                from_s.set_base_hist_dir_long(stof(parts[i+1]));
                                from_s.set_last_wind_time(db_timestamp.str());
                            }
                            catch (exception) { cerr << "!"; }
						}
					}


#ifndef NO_TAP
					frame_type = mac_payload_type::BASE_SENSOR_DATA;
#endif // NO_TAP
					cerr << ".";
					gui.update_status_win();
					flush(cerr);
                    DataOutput::update_all(&from_s);
					break;
				}

				case mac_extheader_types::EXTHDR_DONGLE_CMD_ANSWER:
                    {
                        uint8_t seqno = next_hdr->data[0];
                        uint8_t ack = next_hdr->data[1];

                        if (ack == 0) {
                            COLOR_OK();
                            cout << "Dongle command (" <<
                                setfill('0') << setw(2) << std::hex <<
                                (unsigned int)(frame.mhr.src_address[0]) << ":" <<
                                setfill('0') << setw(2) << std::hex <<
                                (unsigned int)(frame.mhr.src_address[1]) <<
                                ", " << (((unsigned int)seqno) & 0xFF)
                                << "): ACK." << endl;
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

#ifndef NO_TAP
        if (arg_parser.get().use_tap) {
			// construct ethernet frame and write to tap device
			char ether_frame[sizeof(struct ether_header) + frame.payload_size];
			memset(ether_frame, 0, sizeof(ether_frame));

			struct ether_header* ether_hdr = (struct ether_header*)(ether_frame);
			char* data = (char*)(ether_frame + sizeof(struct ether_header));

			memset(ether_hdr->ether_dhost, 0, sizeof(ether_hdr->ether_dhost));
			memset(ether_hdr->ether_shost, 0, sizeof(ether_hdr->ether_shost));

			ether_hdr->ether_type = ether_type;

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

			// send ethernet frame
			if (frame_type == mac_payload_type::ETHERFRAME) {
				memcpy(data, frame.payload, frame.payload_size);
				ptr_tap->send_packet(ether_frame, sizeof(ether_frame));
			}
        }
#endif // NO_TAP

        // cleanup mac packet
		free(frame.payload);
		mac_frame_extheaders_free(frame.extheader);

	}

	// cleanup usb packet
	delete[] pkt.payload;
}



