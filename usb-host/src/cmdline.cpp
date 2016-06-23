/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "cmdline.h"

#include <getopt.h>
#include <unistd.h>


using namespace std;


cmdline::cmdline() { }
cmdline::~cmdline() {}


cmdargs& cmdline::get() {
	return args;
}

void cmdline::parse(int argc, char** argv) {
	this->argc = argc;
	this->argv = argv;

	int c = 0;

    static struct option long_options[] = {
        {"verbose",  no_argument,       0, 'v'},
        {"help",     no_argument,       0, 'h'},
        {"tty",      required_argument, 0, 't'},
        {"tap",      required_argument, 0, 'a'},
        {"no-db",    no_argument, 		0, MAGIC_NO_DB},
        {"no-tap",   no_argument, 		0, MAGIC_NO_TAP},
		{"debug-tap",no_argument, 		0, MAGIC_TAP_DEBUG},
		{"debug-usb",no_argument, 		0, MAGIC_USB_DEBUG},
		{"reset",    no_argument,       0, 'r'},
        {"flash",    no_argument,       0, 'f'},
        {"color",    no_argument,       0, 'c'},
        {"set-mac",  required_argument, 0, MAGIC_SET_MAC},
        {"set-ip",   required_argument, 0, MAGIC_SET_IP},
        {"get-mac",  no_argument,       0, MAGIC_GET_MAC},
        {"get-ip",   no_argument,       0, MAGIC_GET_IP},
        {"calib-compass", required_argument, 0, MAGIC_CALIB_COMPASS},
        {"calib-compass-stop", required_argument, 0, MAGIC_CALIB_COMPASS_STOP},
        {"no-color", no_argument,       0, MAGIC_NO_COLOR},
        {0, 0, 0, 0}
    };

    int opt_index = 0; // getopt_long stores the option index here.
	while ((c = getopt_long(argc, argv, "rcvh?ft:a:", long_options, &opt_index)) != -1) {
		switch (c) {
			case 't':
				args.tty = string(optarg);
				break;
			case 'a':
				args.tap = string(optarg);
				break;
			case 'r':
				args.reset = true;
				break;
			case 'f':
				args.flash = true;
				break;
			case 'v':
				args.verbosity++;
				break;
			case 'c':
				args.prompt_colored = true;
				break;
			case MAGIC_NO_COLOR:
				args.prompt_colored = false;
				break;
			case MAGIC_NO_TAP:
				args.use_tap = false;
				break;
			case MAGIC_NO_DB:
				args.use_db = false;
				break;
			case MAGIC_TAP_DEBUG:
				args.tap_debug = true;
				break;
			case MAGIC_USB_DEBUG:
				args.usb_debug = true;
				break;
			case MAGIC_SET_MAC:
				args.set_mac = string(optarg);
				break;
			case MAGIC_SET_IP:
				args.set_ip = string(optarg);
				break;
			case MAGIC_GET_MAC:
				args.get_mac = true;
				break;
			case MAGIC_GET_IP:
				args.get_ip = true;
				break;
			case MAGIC_CALIB_COMPASS:
				args.calib_compass = string(optarg);
				break;
			case MAGIC_CALIB_COMPASS_STOP:
				args.calib_compass_stop = string(optarg);
				break;
			case 'h':
			case '?':
				printUsage(argc, argv);
				exit(0);
				break;
		}
	}
}

void cmdline::printUsage(int argc, char** argv) {
	cerr << "Usage: " << argv[0] << " [-r] [-n] [-f] [-t /dev/tty*] [-h] [-?]" << endl;
	cerr << endl;
	cerr << "-t, --tty          Specifies which terminal to use for serial connection." << endl;
	cerr << "                   (default: /dev/ttyACM0)" << endl;
	cerr << "-f, --flash        Tell connected dongle to enter bootloader to flash new firmware." << endl;
	cerr << "-r, --reset        Tell connected dongle to reset." << endl;
	cerr << "-t, --tap          Specifies which tap interface to use." << endl;
	cerr << "                   (default: tapsn0)" << endl;
	cerr << "-c, --color        Color console output. (default)" << endl;
	cerr << "--set-mac <addr>   Set MAC address of connected device." << endl;
	cerr << "--set-ip <addr>    Set IP address of connected device." << endl;
	cerr << "--get-mac          Get MAC address of connected device." << endl;
	cerr << "--get-ip           Get IP address of connected device." << endl;
	cerr << "--calib-compass    Send a request to calibrate the compass of device" << endl;
	cerr << "           <addr>  with the given address" << endl;
	cerr << "--calib-compass    Terminates the running compass calibration of device" << endl;
	cerr << "     -stop <addr>  with the given address" << endl;
	cerr << "--no-color         Do not color console output." << endl;
	cerr << "-h, -?, --help     Print this usage message." << endl;
	cerr << endl;
}


void cmdline::printArgs() {
	cerr << "verbosity:       " << args.verbosity << endl;
	cerr << "colored prompt:  " << args.prompt_colored << endl;
	cerr << "tty:             " << args.tty << endl;
	cerr << "tap:             " << args.tap << endl;
	cerr << "goto bootloader: " << args.flash << endl;
	cerr << "tap debug:       " << args.tap_debug << endl;
	cerr << endl;
}
