/*
 * usbtty.cpp
 *
 *  Created on: 12.11.2015
 *      Author: michi
 */

#include "usbtty.h"

#include <stdint.h>
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
static queue<usb_message> txq;
static mutex tx_mtx;

static string tty_path;

usb_tty::usb_tty(string path, void(*rxh)(usb_message)) : rxHandler(rxh) {
	tty_fd = open(path.c_str(), O_RDWR | O_NOCTTY );
    tty_path = path;
	//ioctl(tty_fd, );
}

usb_tty::~usb_tty() {
	close(tty_fd);
}


void usb_tty::usbSendMessage(usb_message msg) {
    tx_mtx.lock();
    txq.push(msg);
    tx_mtx.unlock();
}

void usb_tty::usbTransmitMessage(usb_message msg) {
	usb_message* msg_ptr = &msg;
	msg.magic = USB_MAGIC_NUMBER;
	write(tty_fd, ((char*)(msg_ptr)), 8);
	write(tty_fd, msg.payload, msg.payload_length);
}

void usb_tty::usb_tty_tx_worker(void) {
    while(true) {
//        int last_len = 100;
        tx_mtx.lock();
        if (!txq.empty()) {
            usb_message &m = txq.front();
//            last_len = m.payload_length;
            this->usbTransmitMessage(m);
            delete[] m.payload;
            txq.pop();
        }
        tx_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void usb_tty::usb_tty_rx_worker(void) {
	while(true) {
		char first = 0;
		bool reverse = false;
		//read(tty_fd, &first, 1);
		read(tty_fd, &first, 1);


		if (first == 0) {
			cerr << "usb_tty stream not good: EOF." << endl;
            close(tty_fd);
            while ((tty_fd = open(tty_path.c_str(), O_RDWR | O_NOCTTY)) <= -1) {
                cerr << "Could not reconnect. Waiting..." << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(2500));
            }
            continue;
			//throw(202);
		}


		// wait for first (magic) character
		if ((unsigned char)first != USB_MAGIC_BYTE1 && (unsigned char)first != USB_MAGIC_BYTE2) {
			if (first == 10) continue;
			cerr << "Unexpected character received from USB: " << (int)first <<
                " (0x" << std::hex << (int)first << ")" << std::dec << endl;
		}
		else {
			char expect = 0;
			if ((unsigned char)first == USB_MAGIC_BYTE2) {
				expect = USB_MAGIC_BYTE1;
				reverse = true;
			}
			else if ((unsigned char)first == USB_MAGIC_BYTE1) {
				expect = USB_MAGIC_BYTE2;
				reverse = false;
			}

			// now try to receive second byte
			char second = 0;
			read(tty_fd, &second, 1);

			if (second == expect) {
				// now try to receive packet
				char raw_type;
				read(tty_fd, &raw_type, 1);

				char raw_seqno;
				read(tty_fd, &raw_seqno, 1);

				uint8_t raw_length[4];
				uint32_t length = 0;
				read(tty_fd, raw_length, 4);

				if (reverse) {
					length = length + (uint32_t)raw_length[0];
					length = length + ((uint32_t)raw_length[1] << 8);
					length = length + ((uint32_t)raw_length[2] << 16);
					length = length + ((uint32_t)raw_length[3] << 24);
				}
				else {
					length = length + (uint32_t)raw_length[3];
					length = length + ((uint32_t)raw_length[2] << 8);
					length = length + ((uint32_t)raw_length[1] << 16);
					length = length + ((uint32_t)raw_length[0] << 24);
				}

				if (length <= USB_MAX_PAYLOAD_LENGTH) {
					// and now receive
					//read(tty_fd, rx_buf, length);

					uint8_t* ptr = rx_buf;
					for (uint32_t i = 0; i < length; ++i) {
						read(tty_fd, ptr++, 1);
					}

					usb_message pkt;
					pkt.payload_length = length;
					pkt.type = static_cast<usb_packet_type>(raw_type);
					pkt.seqno = raw_seqno;

					char* payload_buf = new char[pkt.payload_length];
					memcpy(payload_buf, rx_buf, pkt.payload_length);
					pkt.payload = payload_buf;

					rxHandler(pkt);
				}
				else {
					// packet too long
					cerr << "Expecting too long USB packet, waiting for next packet. (" << length << ")" << endl;
                    /*
					char b;
					for (unsigned int i = 0; i < length; ++i) {
						read(tty_fd, &b, 1);
					}
                    */
				}
			}
			else {
				cerr << "Unexpected character received from USB: " << (int)second << endl;
			}
		}
	}
}
