/*
 * error_handling.c
 *
 *  Created on: 08.04.2016
 *      Author: michi
 */

#include "error_handler.h"


error_handler::error_handler() : messages() {

	messages[101] = "Tap device could not be opened.";
	messages[102] = "Tap device could not be opened.";
	messages[201] = "usb tty could not be opened.";
	messages[202] = "usb tty was closed. Perhaps the device was disconnected?";
	messages[301] = "Database connection failed.";
	messages[901] = "Invalid Argument.";

}

std::string error_handler::get_message(int id) {
	if (messages.find(id) == messages.end()) return "General error.";
	return messages[id];
}
