/**
 * @file
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include <getopt.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string.h>

#include <thread>         // std::thread (C++11!)
#include <vector>

#include <usb/message.h>
#include <mac/mac.h>

#include "tap.h"
#include "usbtty.h"

#include <net/ethernet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <linux/ip.h>


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
#define MAGIC_TAP_DEBUG		512

string cmd_tap = "tapsn0";
string cmd_tty = "/dev/ttyACM0";
bool cmd_flash = false;
bool cmd_reset = false;
bool prompt_colored = true;
bool tap_debug = false;
int verbosity = 0;

tap* ptr_tap;
usb_tty* ptr_tty;

void parseCmd(int argc, char** argv);

/**
 * @brief	Will be called when a usb message was received.
 */
void usbReceiveHandler(usb_message msg);

/**
 * @brief	Will be called when a ethernet frame was received on tap interface.
 */
void tapReceiveHandler(void* msg, size_t length);

void do_tap_debug(string);

int main(int argc, char** argv) {
	parseCmd(argc, argv);

	if (verbosity > 0) {
		cerr << "verbosity:       " << verbosity << endl;
		cerr << "colored prompt:  " << prompt_colored << endl;
		cerr << "tty:             " << cmd_tty << endl;
		cerr << "tap:             " << cmd_tap << endl;
		cerr << "goto bootloader: " << cmd_flash << endl;
		cerr << "tap debug:       " << tap_debug << endl;
		cerr << endl;
	}

	// init serial port on linux systems
	//string init = "stty -F " + cmd_tty + " sane raw pass8 -echo -hupcl clocal 115200";
	//string init = "stty -F " + cmd_tty + " raw pass8 -hupcl clocal 115200";
	string init = "stty -F " + cmd_tty + " raw pass8 -echo -hupcl clocal 115200";

	int s = system(init.c_str());
	if (s != 0) {
		COLOR_ERR();
		cerr << "Could not configure serial port  " << cmd_tty << " . Aborting." << endl;
		COLOR_RESET();
		return -1;
	}

	// create usb_tty object and start rx thread
	usb_tty tty(cmd_tty, usbReceiveHandler);
	ptr_tty = &tty;

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

	// create tap object and start rx thread
	try {
		tap tap(cmd_tap, tapReceiveHandler);
		ptr_tap = &tap;
		cerr << "Tap device  " << cmd_tap << "  opened." << endl;

		std::thread usb_rx_thread(&usb_tty::usb_tty_rx_worker, &tty);
		std::thread tap_rx_thread(&tap::tap_rx_worker, &tap);


		// you can do fancy stuff in here


		// wait for thread to exit
		usb_rx_thread.join();
		tap_rx_thread.join();
	} catch (int i) {
		COLOR_ERR();
		cerr << "ERROR: " << i << endl;
		cerr << "Is it possible that you forgot to call setup-tap.sh " <<
				"with correct user name and group?" << endl;
		exit(-1);
		COLOR_RESET();
	}
	return 0;
}

void printUsage(int argc, char** argv) {
	cerr << "Usage: " << argv[0] << " [-r] [-n] [-f] [-t /dev/tty*] [-h] [-?]" << endl;
	cerr << endl;
	cerr << "-t, --tty      Specifies which terminal to use for serial connection." << endl;
	cerr << "               (default: /dev/ttyUSB0)" << endl;
	cerr << "-f, --flash    Tell connected dongle to enter bootloader to flash new firmware." << endl;
	cerr << "-r, --reset    Tell connected dongle to reset." << endl;
	cerr << "-t, --tap      Specifies which tap interface to use." << endl;
	cerr << "               (default: tapsn0)" << endl;
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
        {"tap",      required_argument, 0, 'a'},
        {"debug-tap",no_argument, 		0, MAGIC_TAP_DEBUG},
        {"reset",    no_argument,       0, 'r'},
        {"flash",    no_argument,       0, 'f'},
        {"color",    no_argument,       0, 'c'},
        {"no-color", no_argument,       0, MAGIC_NO_COLOR},
        {0, 0, 0, 0}
    };

    int opt_index = 0; // getopt_long stores the option index here.
	while ((c = getopt_long(argc, argv, "rcvh?ft:a:", long_options, &opt_index)) != -1) {
		switch (c) {
			case 't':
				cmd_tty = string(optarg);
				break;
			case 'a':
				cmd_tap = string(optarg);
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
			case MAGIC_TAP_DEBUG:
				tap_debug = true;
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

void tapReceiveHandler(void *pkt, size_t nread) {
	char *p = (char*)pkt;
	struct ether_header* frame = (struct ether_header*)(p);

	// IPv4 data: ETHERTYPE_IP == ntohs(frame->ether_type)
	if (ETHERTYPE_IP != ntohs(frame->ether_type)) return;

	cerr << "src addr:  " << ether_ntoa((struct ether_addr*)frame->ether_shost) << endl;
	cerr << "dest addr: " << ether_ntoa((struct ether_addr*)frame->ether_dhost) << endl;
	cerr << "type:      " << ntohs(frame->ether_type) << endl;


	struct iphdr* iph = (struct iphdr*)(p + sizeof(struct ether_header));
	cerr << "src ip:    " << inet_ntoa(*(struct in_addr*)&iph->saddr) << endl;
	cerr << "dest ip:   " << inet_ntoa(*(struct in_addr*)&iph->daddr) << endl;
	cerr << "version:   " << (int)(iph->version) << endl;
	cerr << "protocol:  " << (int)(iph->protocol) << endl;

	// data:
	cerr << "data len:  " << (int)(nread - sizeof(struct ether_header) - sizeof(struct iphdr)) << endl;

	cerr << "dest ip:   " << (int)((iph->daddr & 0xff000000) >> 24) << endl;
	cerr << "dest ip:   " << (int)((iph->daddr & 0x00ff0000) >> 16) << endl;
	return;

	for (unsigned int i = 0; i < nread; ++i) {
		cout << p[i];
	}
	cout << endl;


	size_t data_len = nread - sizeof(struct ether_header) - sizeof(struct iphdr);
	if (USB_MAX_PAYLOAD_LENGTH < data_len) {
		COLOR_ERR();
		cerr << "Packet received via network too large for usb tty. Ignoring." << endl;
		COLOR_RESET();
		return;
	}

	// construct real skynet-mac-packet
	mac_frame_data mac_frame;
	mac_frame_data_init(&mac_frame);
	mac_frame.payload = (uint8_t*)(p + sizeof(struct ether_header) + sizeof(struct iphdr));
	mac_frame.payload_size = nread;

	MHR_FC_SET_DEST_ADDR_MODE(mac_frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
	mac_frame.mhr.dest_pan_id[0] = ((iph->daddr & 0x00ff0000) >> 16);		// TODO so herum?
	mac_frame.mhr.address[0] = ((iph->daddr & 0xff000000) >> 24);
	mac_frame.mhr.address[1] = 0;

	uint8_t payload[4096];
	int mac_cnt = mac_frame_data_pack(&mac_frame, payload);

	usb_message m;
	m.type = USB_SKYNET_PACKET;
	m.payload_length = mac_cnt;
	m.payload = (char*)payload;

	ptr_tty->usbSendMessage(m);
}
