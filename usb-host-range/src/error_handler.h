/*
 * error_handling.h
 *
 *  Created on: 08.04.2016
 *      Author: michi
 */

#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

#include <map>
#include <string>

class error_handler {
	public:
		error_handler(void);
		std::string get_message(int);

	private:
		std::map<int, std::string> messages;

};



#endif /* ERROR_HANDLER_H_ */
