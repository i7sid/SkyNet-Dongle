/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <iostream>
#include <string>

#define MAGIC_NO_COLOR		513
#define MAGIC_TAP_DEBUG		512
#define MAGIC_USB_DEBUG		511
#define MAGIC_SET_MAC		510
#define MAGIC_SET_IP 		509
#define MAGIC_GET_MAC		508
#define MAGIC_GET_IP 		507
#define MAGIC_CALIB_COMPASS 506
#define MAGIC_CALIB_COMPASS_STOP 505
#define MAGIC_NO_TAP		504
#define MAGIC_NO_DB 		503



typedef struct cmdargs {
	std::string tap 				= "tapsn0";
	std::string tty 				= "/dev/ttyACM0";
	bool 		flash 				= false;
	bool 		reset 				= false;
	bool 		prompt_colored 		= true;
	bool 		tap_debug 			= false;
	bool 		usb_debug 			= false;
	std::string set_mac 			= "";
	std::string set_ip 				= "";
	bool 		get_mac 			= false;
	bool 		get_ip 				= false;
	bool 		use_tap				= true;
	bool 		use_db				= true;
	std::string calib_compass 		= "";
	std::string calib_compass_stop 	= "";
	int 		verbosity 			= 0;
} cmdargs;

class cmdline {
public:
	cmdline();
	virtual ~cmdline();
	void parse(int argc, char** argv);
	void printUsage(int argc, char** argv);
	void printArgs();

	/*const */ cmdargs& get() /* const */;

protected:

	int argc;
	char** argv;
	cmdargs args;
};

#endif /* CMDLINE_H_ */
