/*
 * tap.cpp
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#ifndef NO_TAP

#include "tap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <string.h>
#include <thread>         // std::thread (C++11!)
#include <vector>

using namespace std;

tap::tap(std::string dev, void(*rxHandler)(void*, size_t)) :
			dev_name(dev), rxHandler(rxHandler) {
	struct ifreq ifr;
	int err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		throw 101;
	}

	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	//ifr.ifr_flags = IFF_TUN;
	if (!dev.empty()) strncpy(ifr.ifr_name, dev.c_str(), dev.length());

	if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
		close(fd);
		throw 102; // TODO throw error
	}
	dev_name = string(ifr.ifr_name);

}

tap::~tap() {
	close(get_fd());
}

int tap::get_fd() {
	return fd;
}

void tap::tap_rx_worker(void) {
	char buffer[4196];
	int nread = 0;

	while (true) {
		// Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes
		nread = read(get_fd(), buffer, sizeof(buffer));
		if(nread < 0) {
			perror("Reading from interface");
			close(get_fd());
			exit(1);
		}

		// Do whatever with the data
		//cerr << "Read " << nread << " bytes from device." << endl;

		// TODO: weiter zerlegen?
		//rxHandler(buffer+4, nread-4);
		rxHandler(buffer, nread);
	}
}

void tap::send_packet(char* data, size_t length) {
	size_t written = write(get_fd(), data, length);
    if (written != length) {
        std::cerr << "Warning: Not all bytes could be sent to tap device." << std::endl;
        std::cerr << "Lost " << (length - written) << " bytes."<< std::endl;
    }
}

#endif // NO_TAP
