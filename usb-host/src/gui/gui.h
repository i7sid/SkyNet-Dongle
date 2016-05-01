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

#include <ncurses.h>
#include <menu.h>


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


typedef struct menuitem {
	std::string name;
	void (*func)(void);
} menuitem;

class gui {
public:
	gui();
	~gui();

	void init();
	void worker();

	void log(std::string);
	void debug(std::string);
	void error(std::string);

private:
    WINDOW* log_border_win;
	WINDOW* cmd_win;
    MENU *menu;
    ITEM *cur_item;
    menuitem items[7];

};

class ncursesbuf: public std::streambuf {
    public:
        ncursesbuf();
        virtual int overflow(int c);
};


class ncurses_stream : public std::ostream {
    public:
        std::ostream &src_;
        std::streambuf * const srcbuf_;
        ncursesbuf tbuf_;

        ncurses_stream(std::ostream &o);
        ~ncurses_stream();
};

#endif /* GUI_H_ */
