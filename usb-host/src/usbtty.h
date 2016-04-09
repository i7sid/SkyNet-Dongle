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
#include "usb/message.h"

class usb_tty {
public:
	usb_tty(std::string path, void(*rxHandler)(usb_message));
	virtual ~usb_tty();

	void usb_tty_rx_worker(void);
	void usbSendMessage(usb_message msg);

private:
	//std::fstream tty;
	int tty_fd;
	void (*rxHandler)(usb_message);
};

#endif /* USBTTY_H_ */
