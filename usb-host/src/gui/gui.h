/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef GUI_H_
#define GUI_H_

#include <iostream>
#include <string>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR_DBG()			std::cerr << ANSI_COLOR_YELLOW;
#define COLOR_ERR()			std::cerr << ANSI_COLOR_RED;
#define COLOR_OK()			std::cerr << ANSI_COLOR_GREEN;
#define COLOR_RESET()		std::cerr << ANSI_COLOR_RESET;


class gui {
public:
	gui();

	void log(std::string);
	void debug(std::string);
	void error(std::string);
};


#endif /* GUI_H_ */
