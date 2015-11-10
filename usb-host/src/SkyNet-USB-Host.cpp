/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include <stdint.h>
#include <unistd.h>
#include <usb/message.h>
#include <iostream>
#include <fstream>

#include <string.h>

#include <thread>         // std::thread (C++11!)


using namespace std;

string cmd_tty = "/dev/ttyUSB0";
char rx_buf[USB_MAX_PAYLOAD_LENGTH];

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message *pkt);


void usbReceiveWorker() {
	fstream tty(cmd_tty.c_str(), ios::in | ios::out | ios::binary);

	while (tty.good()) {
		char first = 0;
		bool reverse = false;
		tty.read(&first, 1);

		// wait for first (magic) character
		if (first != USB_MAGIC_BYTE1 && first != USB_MAGIC_BYTE2) {
			cerr << "Unexpected character received from USB: " << (int)first << endl;
		}
		else {
			char expect = 0;
			if (first == USB_MAGIC_BYTE2) {
				expect = USB_MAGIC_BYTE1;
				reverse = true;
			}
			else if (first == USB_MAGIC_BYTE1) {
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
					pkt.payload = rx_buf;

					usbReceiveHandler(&pkt);
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

int main(int argc, char** argv) {
	parseCmd(argc, argv);

	std::thread usb_thread(usbReceiveWorker);


	usb_thread.join();

	/*
	usb_message m;
	m.magic = 12345;
	char p[2];
	m.payload = p;
	m.payload_length = strlen(p);
	m.type = USB_DEBUG;
	 */


	return 0;
}


void parseCmd(int argc, char** argv) {
	int c = 0;
	while ((c = getopt (argc, argv, "t:")) != -1) {
		switch (c) {
			case 't':
				cmd_tty = string(optarg);
				break;
		}
	}
}

void usbReceiveHandler(usb_message *pkt) {
	cout << "Received USB message: " << endl;
	cout << "Type:\t" << pkt->type << endl;
	cout << "Length:\t" << pkt->payload_length << endl;
	cout << "Payload:" << endl;
	cout << (int)pkt->payload[0] << " "
			 << (int)pkt->payload[1] << " "
			 << (int)pkt->payload[2] << " "
			 << (int)pkt->payload[3] << " "
			 << (int)pkt->payload[4] << " "
			 << (int)pkt->payload[5] << " "
			 << (int)pkt->payload[6] << " "
			 << (int)pkt->payload[7] << " "
			 << (int)pkt->payload[8] << " "
			 << endl << endl;
}

