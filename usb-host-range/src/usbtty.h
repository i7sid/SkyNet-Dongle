/*
 * usbtty.h
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#ifndef USBTTY_H_
#define USBTTY_H_

#include <string>
#include <fstream>

class usb_tty {
public:
	usb_tty(std::string path, void(*rxHandler)(std::string));
	virtual ~usb_tty();

	void usb_tty_rx_worker(void);

private:
	//std::fstream tty;
	int tty_fd;
	FILE* tty_ffd;
    std::string tty_path;
	void (*rxHandler)(std::string);
};

#endif /* USBTTY_H_ */
