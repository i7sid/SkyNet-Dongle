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

#include <usb/message.h>
#include <mac/mac.h>
#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"
#include "cmdline.h"
#include "gui/gui.h"
#include "db/db.h"


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

#ifndef NO_DB
#include "db/db.h"
extern db* ptr_db;
#endif // NO_DB

extern std::ofstream of_wind;
extern std::ofstream of_pos;

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

vector<string> split(const char *str, char c = ' ')
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        string tmp(string(begin, str));
        result.push_back(trim(tmp));
    } while (0 != *str++);

    return result;
}

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

		//cerr << "  "  << (unsigned int)pkt.payload_length << endl;
#ifndef NO_TAP
		mac_payload_type frame_type = mac_payload_type::NONE;
        uint16_t ether_type = 0;
#endif // NO_TAP

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

					// build mac address string
					stringstream mac_builder;
					mac_builder << setfill('0') << setw(2) << std::hex <<
							(unsigned int)(frame.mhr.src_address[0]) << ":" <<
							setfill('0') << setw(2) << std::hex <<
							(unsigned int)(frame.mhr.src_address[1]);

					time_t t = time(0);   // get time now
					struct tm * now = localtime(&t);
					stringstream db_timestamp;
					db_timestamp << setfill('0') << setw(4)
							<< now->tm_year + 1900 << "-" << setfill('0') << setw(2) << now->tm_mon + 1
							<< "-" << setfill('0') << setw(2) << now->tm_mday
							<< " " << parts[0][0] << parts[0][1] << ":"
							<< parts[0][2] << parts[0][3] << ":"
							<< parts[0][4] << parts[0][5];

#ifndef NO_DB
					int station = ptr_db->get_station(mac_builder.str());
#endif // NO_DB


					for (int i = 0; i < next_hdr->typelength_union.type_length.length; ++i) {
						if (next_hdr->data[i] == SENSOR_POSITION) {
#ifndef NO_DB
							ptr_db->record_entity(station, DB_TYPE_GPS, db_timestamp.str(), parts[i+1]);
#endif
							gui.val_pos = parts[i+1];
							cerr << "#";

							of_pos << "\"" << db_timestamp.str() << "\",\"" << mac_builder.str() << "\",\"" <<
									(unsigned int)(next_hdr->data[i]) << "\",\"" << parts[i+1] << "\"" << endl;

						}
						else if (next_hdr->data[i] == SENSOR_COMPASS) {
#ifndef NO_DB
							ptr_db->record_entity(station, DB_TYPE_COMPASS, db_timestamp.str(), parts[i+1]);
#endif // NO_DB
							gui.val_compass = parts[i+1];
							of_pos << "\"" << db_timestamp.str() << "\",\"" << mac_builder.str() << "\",\"" <<
									(unsigned int)(next_hdr->data[i]) << "\",\"" << parts[i+1] << "\"" << endl;
						}
						else if (next_hdr->data[i] == SENSOR_DATE) {
							cerr << "Date: " << parts[i+1] << endl;
						}
						else if (next_hdr->data[i] == SENSOR_WIND_SPEED) {
#ifndef NO_DB
							ptr_db->record_entity(station, DB_TYPE_WIND_SPEED, db_timestamp.str(), parts[i+1]);
#endif // NO_DB
							of_wind << "\"" << db_timestamp.str() << "\",\"" << mac_builder.str() << "\",\"" <<
									(unsigned int)(next_hdr->data[i]) << "\",\"" << parts[i+1] << "\"" << endl;
							gui.val_windspeed = parts[i+1];
						}
						else if (next_hdr->data[i] == SENSOR_WIND_DIR) {
#ifndef NO_DB
							ptr_db->record_entity(station, DB_TYPE_WIND_DIR_COMP, db_timestamp.str(), parts[i+1]);
#endif // NO_DB
							gui.val_winddir = parts[i+1];
							of_wind << "\"" << db_timestamp.str() << "\",\"" << mac_builder.str() << "\",\"" <<
									(unsigned int)(next_hdr->data[i]) << "\",\"" << parts[i+1] << "\"" << endl;
						}
						else if (next_hdr->data[i] == SENSOR_WIND_DIR_RAW) {
#ifndef NO_DB
//							ptr_db->record_entity(station, DB_TYPE_WIND_DIR_RAW, db_timestamp.str(), parts[i+1]);
#endif // NO_DB
							gui.val_winddir_raw = parts[i+1];
							of_wind << "\"" << db_timestamp.str() << "\",\"" << mac_builder.str() << "\",\"" <<
									(unsigned int)(next_hdr->data[i]) << "\",\"" << parts[i+1] << "\"" << endl;
						}
					}


#ifndef NO_TAP
					frame_type = mac_payload_type::BASE_SENSOR_DATA;
#endif // NO_TAP
					cerr << ".";
					gui.update_status_win();
					flush(cerr);
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



