/*
 * usbtty.cpp
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#include "usbtty.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <usb/message.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <string.h>

#include <thread>         // std::thread (C++11!)
#include <vector>

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

#include <queue>
#include <mutex>


using namespace std;

uint8_t rx_buf[USB_MAX_PAYLOAD_LENGTH];


bool file_exists(const char *filename) {
    struct stat buffer;   
    return (stat(filename, &buffer) == 0);
}

usb_tty::usb_tty(string path, void(*rxh)(string)) : tty_path(path),rxHandler(rxh) {
	//ioctl(tty_fd, );
}

usb_tty::~usb_tty() {
	close(tty_fd);
}


void usb_tty::usb_tty_rx_worker(void) {
    while(true) {

        if (!file_exists(tty_path.c_str())) {
            cerr << tty_path << "  is not connected." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

#ifndef __CYGWIN__
		string init = "stty -F " + tty_path + " raw pass8 -echo -hupcl clocal 115200";
		int s = system(init.c_str());
		if (s != 0) {
			cerr << "Could not configure serial port  " << tty_path << " ." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			continue;
        }
#endif


        tty_fd = open(tty_path.c_str(), O_RDWR | O_NOCTTY );
        if (tty_fd == -1) {
            // error: could not open device
            cerr << "Could not open  " << tty_path << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        tty_ffd = fdopen(tty_fd, "r");

        while(true) {
            // TODO readline
            char * line = NULL;
//            char line[];
            size_t len = 0;
            ssize_t read = 0;

            if (feof(tty_ffd)) {
                //fclose(tty_ffd);
                break;
            }
            while ((read = getline(&line, &len, tty_ffd)) != -1) {
                string s = string(line);
                rxHandler(s);
            }

            free(line);
        }

        fclose(tty_ffd);
    }
}
