/*
 * tap.h
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#ifndef TAP_H_
#define TAP_H_

#include <string>
//#include <fstream>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>


class tap {
public:
	tap(std::string dev, void(*rxHandler)(void*, size_t));
	virtual ~tap();
	int get_fd();

	void tap_rx_worker(void);
	void send_packet(char *data, size_t length);

private:
	int fd;
	std::string dev_name;
	void (*rxHandler)(void*, size_t);
};

#endif /* TAP_H_ */
