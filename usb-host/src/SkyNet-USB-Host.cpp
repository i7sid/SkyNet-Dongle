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

#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"

#include <net/ethernet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <linux/ip.h>

#define PKT_DBG_OVERHEAD    (8)

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
#define MAGIC_USB_DEBUG		511

string cmd_tap = "tapsn0";
string cmd_tty = "/dev/ttyACM0";
bool cmd_flash = false;
bool cmd_reset = false;
bool prompt_colored = true;
bool tap_debug = false;
bool usb_debug = false;
int verbosity = 0;

error_handler err;

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
	try {
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
			return 1;
		}

		// create usb_tty object and start rx thread
		usb_tty tty(cmd_tty, usbReceiveHandler);
		ptr_tty = &tty;

		std::thread usb_tx_thread(&usb_tty::usb_tty_tx_worker, &tty);
		
        cerr << "Serial port  " << cmd_tty << " opened." << endl;

		if (cmd_flash) {
			usb_message m;
			char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
			m.type = USB_CONTROL;
			m.payload_length = 1;
			m.payload = payload;
			m.payload[0] = (char)USB_CTRL_BOOTLOADER;

			tty.usbSendMessage(m);
            sleep(1);
			exit(0);
		}
		else if (cmd_reset) {
			usb_message m;
			char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
			m.type = USB_CONTROL;
			m.payload_length = 1;
			m.payload = payload;
			m.payload[0] = (char)USB_CTRL_RESET;

			tty.usbSendMessage(m);
            sleep(1);
			exit(0);
		}

		// create tap object and start rx thread
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
		cerr << err.get_message(i) << endl;

		/*
		 cerr << "Is it possible that you forgot to call setup-tap.sh " <<
				"with correct user name and group?" << endl;
				*/
		exit(1);
		COLOR_RESET();
	}
	return 0;
}

void printUsage(int argc, char** argv) {
	cerr << "Usage: " << argv[0] << " [-r] [-n] [-f] [-t /dev/tty*] [-h] [-?]" << endl;
	cerr << endl;
	cerr << "-t, --tty      Specifies which terminal to use for serial connection." << endl;
	cerr << "               (default: /dev/ttyACM0)" << endl;
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
		{"debug-usb",no_argument, 		0, MAGIC_USB_DEBUG},
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
			case MAGIC_USB_DEBUG:
				usb_debug = true;
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
        COLOR_DBG();
        for (unsigned int i = 0; i < pkt.payload_length; ++i) {
            cout <<  (char)pkt.payload[i];
        }
        COLOR_RESET();
	}
	if (verbosity >= 3) {
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

            ether_hdr->ether_type = ((uint16_t)(frame.mhr.src_address[6]) << 8)
                                        + frame.mhr.src_address[7];
		}

		//ether_hdr->ether_type = htons(ETHERTYPE_IP);

		memcpy(data, frame.payload, frame.payload_size);

//		int frame_length = htons(sizeof(ether_frame) - 4);
//		memcpy(ether_frame, &frame_length, sizeof(int));
		/*
		cerr << "Sending ethernet frame of size " << sizeof(ether_frame) << "." << endl;


		cout << setfill(' ') << setw(3) << std::hex;
        for (unsigned int i = 0; i < sizeof(ether_frame); ++i) {
            cout << "0x" << (((unsigned int)ether_frame[i]) & 0xFF) << " ";
        }
        cout << std::dec << endl;
        */

		ptr_tap->send_packet(ether_frame, sizeof(ether_frame));

/*
		if (verbosity >= 3) {
			cout << "FC0:      " << (int)frame.mhr.frame_control[0] << endl;
			cout << "FC1:      " << (int)frame.mhr.frame_control[1] << endl;
			cout << "dest pan0:" << (int)frame.mhr.dest_pan_id[0] << endl;
			cout << "dest pan1:" << (int)frame.mhr.dest_pan_id[1] << endl;
			cout << "dest add0:" << (int)frame.mhr.dest_address[0] << endl;
			cout << "dest add1:" << (int)frame.mhr.dest_address[1] << endl;
			cout << "src pan 0:" << (int)frame.mhr.src_pan_id[0] << endl;
			cout << "src pan 1:" << (int)frame.mhr.src_pan_id[1] << endl;
			cout << "src  add0:" << (int)frame.mhr.src_address[0] << endl;
			cout << "src  add1:" << (int)frame.mhr.src_address[1] << endl;
			cout << "FCS0:     " << (int)frame.fcs[0] << endl;
			cout << "FCS1:     " << (int)frame.fcs[1] << endl;
			cout << "Payload:  " << (int)frame.payload_size << endl;
			cout << "Payload:  " << frame.payload << endl;
			cout << endl << endl << endl;
		}
*/

		// cleanup mac packet
		free(frame.payload);
	}

	// cleanup usb packet
	delete[] pkt.payload;
}

void tapReceiveHandler(void *pkt, size_t nread) {
	char *p = (char*)pkt;
	struct ether_header* frame = (struct ether_header*)(p);

	// IPv4 data: ETHERTYPE_IP == ntohs(frame->ether_type)
	//if (ETHERTYPE_IP != ntohs(frame->ether_type)) return;

    if (verbosity > 3) {
        cerr << "nread:     " << nread << endl;
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

        for (unsigned int i = 0; i < nread; ++i) {
            cout << p[i];
        }
        cout << endl;
    }

	size_t data_len = nread - sizeof(struct ether_header);
	if (USB_MAX_PAYLOAD_LENGTH < data_len) {
		COLOR_ERR();
		cerr << "Packet received via network too large for usb tty. Ignoring." << endl;
		COLOR_RESET();
		return;
	}

	// construct real skynet-mac-packet
	mac_frame_data mac_frame;
	mac_frame_data_init(&mac_frame);
	mac_frame.payload = (uint8_t*)(p + sizeof(struct ether_header));
	mac_frame.payload_size = data_len;

	MHR_FC_SET_DEST_ADDR_MODE(mac_frame.mhr.frame_control, MAC_ADDR_MODE_LONG);
	mac_frame.mhr.dest_pan_id[0] = 0;
	mac_frame.mhr.dest_pan_id[1] = 0;
	mac_frame.mhr.dest_address[0] = frame->ether_dhost[0];
	mac_frame.mhr.dest_address[1] = frame->ether_dhost[1];
	mac_frame.mhr.dest_address[2] = frame->ether_dhost[2];
	mac_frame.mhr.dest_address[3] = frame->ether_dhost[3];
	mac_frame.mhr.dest_address[4] = frame->ether_dhost[4];
	mac_frame.mhr.dest_address[5] = frame->ether_dhost[5];


	MHR_FC_SET_SRC_ADDR_MODE(mac_frame.mhr.frame_control, MAC_ADDR_MODE_LONG);
	mac_frame.mhr.src_pan_id[0] = 0;
	mac_frame.mhr.src_pan_id[1] = 0;
	mac_frame.mhr.src_address[0] = frame->ether_shost[0];
	mac_frame.mhr.src_address[1] = frame->ether_shost[1];
	mac_frame.mhr.src_address[2] = frame->ether_shost[2];
	mac_frame.mhr.src_address[3] = frame->ether_shost[3];
	mac_frame.mhr.src_address[4] = frame->ether_shost[4];
	mac_frame.mhr.src_address[5] = frame->ether_shost[5];
	mac_frame.mhr.src_address[6] = ((frame->ether_type & 0xFF00) >> 8);
	mac_frame.mhr.src_address[7] = ((frame->ether_type & 0x00FF)     );

	//uint8_t payload[4096];
	uint8_t *payload = new uint8_t[4096];
    memset(payload, 0, 4096);
	int mac_cnt = mac_frame_data_pack(&mac_frame, payload);
	mac_frame_calc_crc(payload, mac_cnt);

	usb_message m;
	m.type = USB_SKYNET_PACKET;
	m.payload_length = mac_cnt + PKT_DBG_OVERHEAD;
	m.payload = (char*)payload;

    /*
    COLOR_DBG();
    cerr << "Packet sending over USB (" << (m.payload_length) << ")" << endl;
    COLOR_RESET();

    cout << setfill(' ') << setw(3) << std::hex;
    for (unsigned int i = 0; i < m.payload_length; ++i) {
        cout << "0x" << (((unsigned int)m.payload[i]) & 0xFF) << " ";
    }
    */


	ptr_tty->usbSendMessage(m);
}

