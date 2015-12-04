/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include <getopt.h>
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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR_DBG()			if (prompt_colored) { cerr << ANSI_COLOR_YELLOW; }
#define COLOR_ERR()			if (prompt_colored) { cerr << ANSI_COLOR_RED; }
#define COLOR_RESET()		if (prompt_colored) { cerr << ANSI_COLOR_RESET; }

#define MAGIC_NO_COLOR		513

string cmd_tty = "/dev/ttyACM0";
bool cmd_flash = false;
bool cmd_reset = false;
bool prompt_colored = true;
int verbosity = 0;

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message msg);


int main(int argc, char** argv) {
	parseCmd(argc, argv);

	if (verbosity > 0) {
		cerr << "verbosity:       " << verbosity << endl;
		cerr << "colored prompt:  " << prompt_colored << endl;
		cerr << "tty:             " << cmd_tty << endl;
		cerr << "goto bootloader: " << cmd_flash << endl;
		cerr << endl;
	}

	// init serial port on linux systems
	//string init = "stty -F " + cmd_tty + " sane raw pass8 -echo -hupcl clocal 115200";
	string init = "stty -F " + cmd_tty + " raw pass8 -hupcl clocal 115200";

	int s = system(init.c_str());
	if (s != 0) {
		COLOR_ERR();
		cerr << "Could not configure serial port  " << cmd_tty << " . Aborting." << endl;
		COLOR_RESET();
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
	else if (cmd_reset) {
		usb_message m;
		char payload[USB_MAX_PAYLOAD_LENGTH];
		m.type = USB_CONTROL;
		m.payload_length = 1;
		m.payload = payload;
		m.payload[0] = (char)USB_CTRL_RESET;

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
	cerr << "Usage: " << argv[0] << " [-r] [-n] [-f] [-t /dev/tty*] [-h] [-?]" << endl;
	cerr << endl;
	cerr << "-t, --tty      Specifies which terminal to use for serial connection." << endl;
	cerr << "               (default: /dev/ttyUSB0)" << endl;
	cerr << "-f, --flash    Tell connected dongle to enter bootloader to flash new firmware." << endl;
	cerr << "-r, --reset    Tell connected dongle to reset." << endl;
	cerr << "-c, --color    Color console output. (default)" << endl;
	cerr << "--no-color     Do not color console output." << endl;
	cerr << "-h, -?, --help Print this usage message." << endl;
	cerr << endl;
}

void parseCmd(int argc, char** argv) {
	int c = 0;

    static struct option long_options[] = {
        {"verbose",  no_argument,       0, 'v'},
        {"help",     no_argument,       0, 'h'},
        {"tty",      required_argument, 0, 't'},
        {"reset",    no_argument,       0, 'r'},
        {"flash",    no_argument,       0, 'f'},
        {"color",    no_argument,       0, 'c'},
        {"no-color", no_argument,       0, MAGIC_NO_COLOR},
        {0, 0, 0, 0}
    };

    int opt_index = 0; // getopt_long stores the option index here.
	while ((c = getopt_long(argc, argv, "rcvh?ft:", long_options, &opt_index)) != -1) {
		switch (c) {
			case 't':
				cmd_tty = string(optarg);
				break;
			case 'r':
				cmd_reset = true;
				break;
			case 'f':
				cmd_flash = true;
				break;
			case 'v':
				verbosity++;
				break;
			case 'c':
				prompt_colored = true;
				break;
			case MAGIC_NO_COLOR:
				prompt_colored = false;
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

	if (pkt.type == USB_DEBUG) {
		if (verbosity >= 1) {
			COLOR_DBG();
			for (unsigned int i = 0; i < pkt.payload_length; ++i) {
				cout <<  (char)pkt.payload[i];
			}
			COLOR_RESET();
		}
	}
	if (verbosity >= 2) {
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
	}

	delete[] pkt.payload;
}

