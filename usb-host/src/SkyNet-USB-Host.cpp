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
bool cmd_flash = false;

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message msg);


int main(int argc, char** argv) {
	parseCmd(argc, argv);

	// init serial port on linux systems
	//string init = "stty -F " + cmd_tty + " sane raw pass8 -echo -hupcl clocal 115200";
	string init = "stty -F " + cmd_tty + " raw pass8 -hupcl clocal 115200";

	int s = system(init.c_str());
	if (s != 0) {
		cerr << "Could not configure serial port  " << cmd_tty << " . Aborting." << endl;
		return -1;
	}

	// creyte usb_tty object and start rx thread
	usb_tty tty(cmd_tty, usbReceiveHandler);

	if (cmd_flash) {
		usb_message m;
		char payload[USB_MAX_PAYLOAD_LENGTH];
		m.type = USB_CONTROL;
		m.payload_length = 1;
		m.payload = payload;
		m.payload[0] = (char)USB_CTRL_BOOTLOADER;

		tty.usbSendMessage(m);
		exit(0);
	}

	/*
	while (true) {
		usb_message m;
		m.type = USB_DEBUG;
		m.payload = (char*)string("test").c_str();
		m.payload_length = 4;
		m.seqno = 0;
		tty.usbSendMessage(m);
		sleep(1);
	}
	*/

	std::thread usb_rx_thread(&usb_tty::usb_tty_rx_worker, &tty);


	// you can do fancy stuff in here


	// wait for thread to exit
	usb_rx_thread.join();

	return 0;
}

void printUsage(int argc, char** argv) {
	cerr << "Usage: " << argv[0] << " [-f] [-t /dev/tty*] [-h] [-?]" << endl;
	cerr << endl;
	cerr << "-t      Specifies which terminal to use for serial connection." << endl;
	cerr << "        (default: /dev/ttyUSB0)" << endl;
	cerr << "-f      Tell connected dongle to enter bootloader to flash new firmware." << endl;
	cerr << "-h, -?  Print this usage message." << endl;
	cerr << endl;
}

void parseCmd(int argc, char** argv) {
	int c = 0;
	while ((c = getopt (argc, argv, "h?ft:")) != -1) {
		switch (c) {
			case 't':
				cmd_tty = string(optarg);
				break;
			case 'f':
				cmd_flash = true;
				break;
			case 'h':
			case '?':
				printUsage(argc, argv);
				exit(0);
				break;
		}
	}
}

void usbReceiveHandler(usb_message pkt) {
	cout << "Received USB message: " << endl;
	cout << "Type:\t" << (int)pkt.type << endl;
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

