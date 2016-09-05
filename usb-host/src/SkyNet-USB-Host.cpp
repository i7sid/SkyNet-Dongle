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
#include <sstream>

#include <usb/message.h>
#include <mac/mac.h>

#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"
#include "cmdline.h"
#include "tap_handler.h"
#include "usb_handler.h"
#include "gui/gui.h"
#include "station.h"
#include "thermal.h"
#include "output/output.h"
#include "output/json.h"
#include "output/csv.h"
#include "output/db.h"


using namespace std;


int verbosity = 0;
bool prompt_colored = true;
error_handler err;
usb_tty* ptr_tty;
cmdline arg_parser;
gui gui;
int local_mac[6];
//JsonOutput out_json("wind.json");
JsonOutput out_json("/var/www/tmp/wind.json");
CsvOutput out_csv;
thermal therm;

#ifndef NO_DB
DbOutput *out_db;
#endif


#ifndef NO_TAP
tap* ptr_tap;
#endif // NO_TAP


void do_tap_debug(string);

int main(int argc, char** argv) {
	try {
		arg_parser.parse(argc, argv);
		cmdargs& args = arg_parser.get();
		verbosity = args.verbosity;
		prompt_colored = args.prompt_colored;

		if (verbosity > 0) {
			arg_parser.printArgs();
		}


#ifndef __CYGWIN__
		// init serial port on linux systems
		//string init = "stty -F " + args.tty + " sane raw pass8 -echo -hupcl clocal 115200";
		//string init = "stty -F " + args.tty + " raw pass8 -hupcl clocal 115200";
		string init = "stty -F " + args.tty + " raw pass8 -echo -hupcl clocal 115200";

		// TODO Überspringen bzw. nicht abstürzen
		int s = system(init.c_str());
		if (s != 0) {
			COLOR_ERR();
			cerr << "Could not configure serial port  " << args.tty << " . Aborting." << endl;
			COLOR_RESET();
			return 1;
		}
#endif

		// create usb_tty object and start rx thread
		usb_tty tty(args.tty, usbReceiveHandler);
		ptr_tty = &tty;


		// initialize gui
		gui.init();
	    ncurses_stream foo(std::cout);
	    ncurses_stream foo2(std::cerr);

	    // get current time
	    time_t t = time(0);
		struct tm * now = localtime(&t);
		stringstream time_string;
		time_string << setfill('0') << setw(4) << now->tm_year + 1900 << "-"
				<< setfill('0') << setw(2) << now->tm_mon + 1 << "-"
				<< setfill('0') << setw(2) << now->tm_mday << "-"
				<< setfill('0') << setw(2) << now->tm_hour << "-"
				<< setfill('0') << setw(2) << now->tm_min << "-"
				<< setfill('0') << setw(2) << now->tm_sec;

	    // output handlers
        out_csv.set_filename("data-" + time_string.str() + ".csv");
        DataOutput::register_output(&out_json);
        DataOutput::register_output(&out_csv);
#ifndef NO_DB
		if (args.use_db) {
            out_db = new DbOutput();
            DataOutput::register_output(out_db);
        }
#endif


        // parallel threads
		std::thread usb_tx_thread(&usb_tty::usb_tty_tx_worker, &tty);
        std::thread usb_rx_thread(&usb_tty::usb_tty_rx_worker, &tty);
        usb_tx_thread.detach();
        usb_rx_thread.detach();
        cerr << "Serial port  " << args.tty << " opened." << endl;

        // parse local mac address
        string local_mac_s(MAC_ADDR);
        if (local_mac_s.length() != 17 ||
				6 != sscanf(local_mac_s.c_str(), "%x:%x:%x:%x:%x:%x",
							&local_mac[0], &local_mac[1], &local_mac[2],
							&local_mac[3], &local_mac[4], &local_mac[5])) {
        	throw 902;
        }

        if (args.set_mac.length() > 0) {
            int values[6];
            if (args.set_mac.length() == 17 &&
                6 == sscanf(args.set_mac.c_str(), "%x:%x:%x:%x:%x:%x",
                            &values[0], &values[1], &values[2],
                            &values[3], &values[4], &values[5])) {

                    usb_message m;
                    char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
                    m.type = USB_CONTROL;
                    m.payload_length = 7;
                    m.payload = payload;
                    m.payload[0] = (char)USB_CTRL_SET_MAC_ADDR;
                    m.payload[1] = (uint8_t)(values[0] & 0xFF);
                    m.payload[2] = (uint8_t)(values[1] & 0xFF);
                    m.payload[3] = (uint8_t)(values[2] & 0xFF);
                    m.payload[4] = (uint8_t)(values[3] & 0xFF);
                    m.payload[5] = (uint8_t)(values[4] & 0xFF);
                    m.payload[6] = (uint8_t)(values[5] & 0xFF);
                    tty.usbSendMessage(m);
                    sleep(1);
                    exit(0);
            }
            else {
                cerr << "MAC address malformed. Please use format   aa:bb:cc:dd:ee:ff ." << endl;
                exit(1);
            }
        }
		else if (args.set_ip.length() > 0) {
            short unsigned int values[4];
            if (args.set_ip.length() <= 15 && args.set_ip.length() >= 7 &&
                4 == sscanf(args.set_ip.c_str(), "%hu.%hu.%hu.%hu",
                            &values[0], &values[1], &values[2], &values[3])) {

                    usb_message m;
                    char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
                    m.type = USB_CONTROL;
                    m.payload_length = 5;
                    m.payload = payload;
                    m.payload[0] = (char)USB_CTRL_SET_IP_ADDR;
                    m.payload[1] = (uint8_t)(values[0] & 0xFF);
                    m.payload[2] = (uint8_t)(values[1] & 0xFF);
                    m.payload[3] = (uint8_t)(values[2] & 0xFF);
                    m.payload[4] = (uint8_t)(values[3] & 0xFF);
                    tty.usbSendMessage(m);
                    sleep(1);
                    exit(0);
            }
            else {
                cerr << "IP address malformed. Please use format   127.0.0.1 ." << endl;
                exit(1);
            }


            exit(0);
        }
		else if (args.calib_compass_stop.length() > 0) {
            int values[2];
            if (args.calib_compass_stop.length() == 5 &&
                2 == sscanf(args.calib_compass_stop.c_str(), "%x:%x",
                            &values[0], &values[1])) {

                    char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
                    mac_frame_data frame;
                    mac_frame_data_init(&frame);

                    frame.payload = NULL;
                    frame.payload_size = 0;

                    MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                    frame.mhr.dest_pan_id[0] = 0;
                    frame.mhr.dest_pan_id[1] = 0;
                    frame.mhr.dest_address[0] = values[0];
                    frame.mhr.dest_address[1] = values[1];

                    MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                    frame.mhr.src_pan_id[0] = 0;
                    frame.mhr.src_pan_id[1] = 0;
                    frame.mhr.src_address[0] = 0; // TODO get local
                    frame.mhr.src_address[1] = 0; // TODO get local


                    // generate extended headers
                    mac_extheader hdr;
                    mac_extheader_init(&hdr);
                    hdr.typelength_union.type_length.length = 1;
                    hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD;
                    hdr.data[0] = dongle_command::CALIB_COMPASS_STOP;

                    frame.extheader = &hdr;

                    uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);


                    usb_message m;
                    m.type = USB_SKYNET_PACKET;
                    m.payload_length = usb_length;
                    m.payload = usb_payload;
                    tty.usbSendMessage(m);

                    sleep(6);
                    exit(0);
            }
            else {
                cerr << "MAC address malformed. Please use format   AA:BB ." << endl;
                throw 901;
            }
        }

		if (args.usb_debug) {
			string dbg("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
			usb_message m;
			m.type = USB_DEBUG;
			m.payload = (char*)dbg.c_str();
			m.payload_length = dbg.length() + 1;
			m.seqno = 0;
			tty.usbSendMessage(m);
			return 0;
		}

#ifndef NO_TAP
		// create tap object and start rx thread
		std::thread* ptr_tap_rx_thread = nullptr;
		if (args.use_tap) {
			ptr_tap = new tap(args.tap, tapReceiveHandler);
			cerr << "Tap device  " << args.tap << "  opened." << endl;

			ptr_tap_rx_thread = new thread(&tap::tap_rx_worker, ptr_tap);
		}
#endif // NO_TAP


		// give visually feedback that we are running
		cerr << "Ready." << endl;
		cerr << "My mac address is  " << MAC_ADDR << " . " << endl;

		// you can do fancy stuff in here
		while (true) {
			gui.worker();
		}


		// wait for thread to exit
		usb_rx_thread.join();
#ifndef NO_TAP
		if (args.use_tap) {
			ptr_tap_rx_thread->join();
			delete ptr_tap;
			delete ptr_tap_rx_thread;
		}
#endif
#ifndef NO_DB
		if (args.use_db) {
            delete out_db;
        }
#endif

	} catch (int i) {
		COLOR_ERR();
		cerr << "ERROR: " << i << endl;
		cerr << err.get_message(i) << endl;

		exit(1);
		COLOR_RESET();
	}
	return 0;
}


