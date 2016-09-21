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
#include "usbtty.h"
#include "cmdline.h"
#include "gui/gui.h"


using namespace std;


int verbosity = 0;
bool prompt_colored = true;
error_handler err;
cmdline arg_parser;
gui gui;


void test1(string);
void test2(string);

string tty1_path = "/dev/sndongle";
string tty2_path = "/dev/snfeather";
string filebase;


int main(int argc, char** argv) {
	try {
		arg_parser.parse(argc, argv);
		cmdargs& args = arg_parser.get();
		verbosity = args.verbosity;
		prompt_colored = args.prompt_colored;

		if (verbosity > 0) {
			arg_parser.printArgs();
		}


		// create usb_tty object and start rx thread
		usb_tty tty1(tty1_path, test1);
		usb_tty tty2(tty2_path, test2);

        //ptr_tty = &tty;
		//ptr_tty2 = &tty;


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
        filebase = "range-" + time_string.str();


        // parallel threads
        std::thread usb_rx_thread1(&usb_tty::usb_tty_rx_worker, &tty1);
        std::thread usb_rx_thread2(&usb_tty::usb_tty_rx_worker, &tty2);
        usb_rx_thread1.detach();
        usb_rx_thread2.detach();
        cerr << "Serial port  opened." << endl;
        cerr << "Serial port  opened." << endl;


		// give visually feedback that we are running
		cerr << "Ready." << endl;

		// you can do fancy stuff in here
		while (true) {
			gui.worker();
		}


		// wait for thread to exit
		usb_rx_thread1.join();
		usb_rx_thread2.join();

	} catch (int i) {
		COLOR_ERR();
		cerr << "ERROR: " << i << endl;
		cerr << err.get_message(i) << endl;

		exit(1);
		COLOR_RESET();
	}
	return 0;
}


void test1(string s) {
    ofstream of;
	of.open(filebase + "-d1.csv", std::ofstream::out | std::ofstream::app);
    of << s;
    of.close();
}
void test2(string s) {
    ofstream of;
	of.open(filebase + "-d2.csv", std::ofstream::out | std::ofstream::app);
    of << s;
    of.close();
}
