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
#include <vector>

#include "usbtty.h"

using namespace std;

string cmd_tty = "/dev/ttyACM0";

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message msg);


int main(int argc, char** argv) {
	parseCmd(argc, argv);

	// init serial port on linux systems
	//string init = "stty -F " + cmd_tty + " sane raw pass8 -echo -hupcl clocal 115200";
	string init = "stty -F " + cmd_tty + " pass8 -hupcl clocal 115200";
	int s = system(init.c_str());
	if (s != 0) {
		cerr << "Could not configure serial port  " << cmd_tty << " . Aborting." << endl;
		return -1;
	}

	// creyte usb_tty object and start rx thread
	usb_tty tty(cmd_tty, usbReceiveHandler);
	std::thread usb_rx_thread(&usb_tty::usb_tty_rx_worker, &tty);


	// you can do fancy stuff in here


	// wait for thread to exit
	usb_rx_thread.join();

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

void usbReceiveHandler(usb_message pkt) {
	cout << "Received USB message: " << endl;
	cout << "Type:\t" << pkt.type << endl;
	cout << "SeqNo:\t" << (int)pkt.seqno << endl;
	cout << "Length:\t" << pkt.payload_length << endl;
	cout << "Payload:" << endl;

	for (unsigned int i = 0; i < pkt.payload_length; ++i) {
		cout << setfill(' ') << setw(3) << (unsigned int)pkt.payload[i] << " ";
	}
	cout << endl;
	for (unsigned int i = 0; i < pkt.payload_length; ++i) {
		cout <<  " " << (char)pkt.payload[i] << "  ";
	}
	cout << endl << endl;


	delete[] pkt.payload;
}

