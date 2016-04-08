/*
 * usbtty.cpp
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#include "usbtty.h"

#include <stdint.h>
#include <unistd.h>
#include <usb/message.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <string.h>

#include <thread>         // std::thread (C++11!)
#include <vector>

using namespace std;

char rx_buf[USB_MAX_PAYLOAD_LENGTH];

usb_tty::usb_tty(string path, void(*rxh)(usb_message)) :
	tty(path.c_str(), ios::in | ios::out | ios::binary), rxHandler(rxh) {

}

usb_tty::~usb_tty() {
	tty.flush();
	tty.close();
}


void usb_tty::usbSendMessage(usb_message msg) {
	usb_message* msg_ptr = &msg;
	msg.magic = USB_MAGIC_NUMBER;

	tty.write(((char*)(msg_ptr)), 8);
	tty.flush();
	tty.write(msg.payload, msg.payload_length);
	tty.flush();
}

void usb_tty::usb_tty_rx_worker(void) {
	if (tty.good()) while(true) {
		char first = 0;
		bool reverse = false;
		tty.read(&first, 1);

		if (first == 0 && !tty.good()) {
			throw(202);
		}

		// wait for first (magic) character
		if ((unsigned char)first != USB_MAGIC_BYTE1 && (unsigned char)first != USB_MAGIC_BYTE2) {
			cerr << "Unexpected character received from USB: " << (int)first << endl;
		}
		else {
			char expect = 0;
			if ((unsigned char)first == USB_MAGIC_BYTE2) {
				expect = USB_MAGIC_BYTE1;
				reverse = true;
			}
			else if ((unsigned char)first == USB_MAGIC_BYTE1) {
				expect = USB_MAGIC_BYTE2;
				reverse = false;
			}

			// now try to receive second byte
			char second = 0;
			tty.read(&second, 1);

			if (second == expect) {
				// now try to receive packet
				char raw_type;
				tty.read(&raw_type, 1);

				char raw_seqno;
				tty.read(&raw_seqno, 1);

				char raw_length[4];
				uint32_t length = 0;
				tty.read(raw_length, 4);

				if (reverse) {
					length = length + (int)raw_length[0];
					length = length + ((int)raw_length[1] << 8);
					length = length + ((int)raw_length[2] << 16);
					length = length + ((int)raw_length[3] << 24);
				}
				else {
					length = length + (int)raw_length[3];
					length = length + ((int)raw_length[2] << 8);
					length = length + ((int)raw_length[1] << 16);
					length = length + ((int)raw_length[0] << 24);
				}

				if (length <= USB_MAX_PAYLOAD_LENGTH) {
					// and now receive
					tty.read(rx_buf, length);

					usb_message pkt;
					pkt.payload_length = length;
					pkt.type = static_cast<usb_packet_type>(raw_type);
					pkt.seqno = raw_seqno;

					char* payload_buf = new char[pkt.payload_length];
					memcpy(payload_buf, rx_buf, pkt.payload_length);
					pkt.payload = payload_buf;

					rxHandler(pkt);
				}
				else {
					// packet too long
					cerr << "Expecting too long USB packet, ignoring whole packet." << endl;
					char b;
					for (unsigned int i = 0; i < length; ++i) {
						tty.read(&b, 1);
					}
				}
			}
			else {
				cerr << "Unexpected character received from USB: " << (int)second << endl;
			}
		}
	}
}
