/*
 * tap.cpp
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#include "tap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#include <thread>         // std::thread (C++11!)
#include <vector>


using namespace std;

tap::tap(std::string dev) {
	struct ifreq ifr;
	int err;

	dev_name = dev;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		throw 0;
		return;
	}

	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = IFF_TAP;
	if (!dev.empty()) strncpy(ifr.ifr_name, dev.c_str(), dev.length());

	if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
		close(fd);
		throw 1; // TODO throw error
	}
	dev_name = string(ifr.ifr_name);
}

tap::~tap() {

}

int tap::get_fd() {
	return fd;
}
