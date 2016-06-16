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
#include <mutex>          // std::mutex

#include <ncurses.h>
#include <menu.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR_DBG()			{ wattr_on(log_win, COLOR_PAIR(2) | A_BOLD, nullptr); }
#define COLOR_ERR()			{ wattr_on(log_win, COLOR_PAIR(1) | A_BOLD, nullptr); }
#define COLOR_OK()			{ wattr_on(log_win, COLOR_PAIR(3) | A_BOLD, nullptr); }
#define COLOR_RESET()		{ wattr_off(log_win, COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BOLD, nullptr); }


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
	void update_status_win();
	void redraw();    
    void update_size();

	void log(std::string);
	void debug(std::string);
	void error(std::string);

	std::string val_compass;
	std::string val_pos;
	std::string val_windspeed;
	std::string val_winddir;
	std::string val_winddir_raw;

protected:

private:

    WINDOW* log_border_win;
    WINDOW* cmd_win;
    WINDOW* status_win;
    MENU *menu;
    ITEM *cur_item;
    menuitem items[32];

};

void print_in_middle(WINDOW *win, int starty, int startx, int width, std::string s, chtype color);

extern WINDOW *log_win;
extern WINDOW *my_form_win;
extern std::mutex log_mtx;           // mutex for critical log output section

class ncursesbuf: public std::streambuf {
    public:
        ncursesbuf() {}
        virtual int overflow(int c) {
            log_mtx.lock();
        	waddch(log_win, c);
        	wrefresh(log_win);

        	if (my_form_win != nullptr) {
        		redrawwin(my_form_win);
        		wrefresh(my_form_win);
        	}
            log_mtx.unlock();
        	return c;
        }

};


class ncurses_stream : public std::ostream {
    public:
        std::ostream &src_;
        std::streambuf * const srcbuf_;
        ncursesbuf tbuf_;


        ncurses_stream(std::ostream &o) : std::ostream(&tbuf_), src_(o), srcbuf_(o.rdbuf()) {
            o.rdbuf(rdbuf());
        }
        ~ncurses_stream() {
        	src_.rdbuf(srcbuf_);
        }
};

#endif /* GUI_H_ */
