/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include <stdint.h>
#include <unistd.h>
#include <usb/message.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <string.h>

#include <thread>         // std::thread (C++11!)


using namespace std;

string cmd_tty = "/dev/ttyACM0";
char rx_buf[USB_MAX_PAYLOAD_LENGTH];

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message *pkt);


void usbReceiveWorker() {
	fstream tty(cmd_tty.c_str(), ios::in | ios::out | ios::binary);

	int n = 0;
	while(true) {
		usb_message msg;
		usb_message* msg_ptr = &msg;
		msg.magic = USB_MAGIC_NUMBER;
		msg.type = 3;
		msg.seqno = n++;
		char p[] = "TEST-Nachricht";
		msg.payload = p;
		msg.payload_length = strlen(p);

		/*
		for (int i=0; i < 8; ++i) {
			tty.write(((char*)(msg_ptr)) + i, 1);
			tty.flush();
		}
		for (unsigned int i=0; i < msg.payload_length; ++i) {
			tty.write(msg.payload+i, 1);
			tty.flush();
		}
		*/
		tty.write(((char*)(msg_ptr)), 8);
		tty.flush();
		tty.write(msg.payload, msg.payload_length);
		tty.flush();



		cout << setfill(' ') << setw(3) <<  (int)msg.seqno << " Sent. " << endl;



		sleep(2);
	}



	while (tty.good()) {
		char first = 0;
		bool reverse = false;
		tty.read(&first, 1);
		//cerr << "Read: " << (int)(uint8_t)first << endl;
		//continue;

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

				char raw_seqno;
				tty.read(&raw_seqno, 1);

				char raw_length[4];
				uint32_t length = 0;
				tty.read(raw_length, 4);

				if (reverse) {
					cerr << (int)raw_length[3] << " " << (int)raw_length[2] << " " << (int)raw_length[1] << " " << (int)raw_length[0] << " " << endl;
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

				cerr << "Test: " << (int)first << "  " << (int)second << "  " << (int)length << endl;

				if (length <= USB_MAX_PAYLOAD_LENGTH) {
					// and now receive
					tty.read(rx_buf, length);

					usb_message pkt;
					pkt.payload_length = length;
					pkt.type = static_cast<usb_packet_type>(raw_type);
					pkt.seqno = raw_seqno;
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

	for (unsigned int i = 0; i < pkt->payload_length; ++i) {
		cout << setfill(' ') << setw(3) << (unsigned int)pkt->payload[i] << " ";
	}
	cout << endl;
	for (unsigned int i = 0; i < pkt->payload_length; ++i) {
		cout <<  " " << (char)pkt->payload[i] << "  ";
	}
	cout << endl << endl;

}

