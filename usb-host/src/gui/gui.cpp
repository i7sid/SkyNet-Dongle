/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "gui.h"

#include <ncurses.h>
#include <menu.h>

using namespace std;



#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char *choices[] = {
                        "Dongle neustarten",
                        "Firmwareupdate starten",
                        "-",
                        "MAC-Adresse anzeigen",
                        "MAC-Adresse setzen",
                        "-",
                        "Beenden",
                  };

WINDOW* log_win;


gui::gui() {}
gui::~gui() {
    endwin();
//    fprintf(stderr, "Program terminated by SIGINT.\n");
}

void gui::init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();

    int menu_width = 36;

    ncurses_stream foo(std::cout);

    log_border_win = newwin(LINES, COLS - menu_width, 0, 0);
    box(log_border_win, 0 , 0);
    wrefresh(log_border_win);

    log_win = newwin(LINES - 2, COLS - menu_width - 2, 1, 1);
    scrollok(log_win, TRUE);
    wrefresh(log_win);

    refresh();

    cmd_win = newwin(LINES, menu_width, 0, COLS - menu_width);
    box(cmd_win, 0 , 0);
    wrefresh(cmd_win);
    keypad(cmd_win, TRUE);

    // menu
    ITEM **my_items;
    int n_choices, i;

    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

    for(i = 0; i < n_choices; ++i) {
        my_items[i] = new_item(choices[i], NULL); //, choices[i]);
    }
    my_items[n_choices] = (ITEM *)NULL;

    menu = new_menu((ITEM **)my_items);
    set_menu_win(menu, cmd_win);
    set_menu_sub(menu, derwin(cmd_win, LINES - 4, menu_width - 4, 2, 2));
    set_menu_format(menu, LINES - 4, 1);
    set_menu_mark(menu, " * ");

    post_menu(menu);
    wrefresh(cmd_win);

    refresh();

}

void gui::worker() {
    int c = getch();
        switch(c) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;

                // bringt nichts, wenn Puffer nicht selbst gefüllt wird
//            case KEY_PPAGE:
//                wscrl(log_win, -5);
//                break;
//            case KEY_NPAGE:
//                wscrl(log_win, 5);
//                break;

            case 10: /* Enter */
            {
                endwin();
                exit(0);

                ITEM *cur;
                //void (*p)(char *);

                cur = current_item(menu);
                //p = item_userptr(cur);
                //p((char *)item_name(cur));

                printw("Hello world! %s %d\n", cur->name, cur->index);

                pos_menu_cursor(menu);
                break;
            }
        }
        wrefresh(cmd_win);
        refresh();
}

void gui::log(std::string s) {
    COLOR_RESET();
    cerr << s;
}
void gui::debug(std::string s) {
    COLOR_DBG();
    cerr << s;
    COLOR_RESET();
}
void gui::error(std::string s) {
    COLOR_ERR();
    cerr << s;
    COLOR_RESET();
}


ncursesbuf::ncursesbuf() {}
int ncursesbuf::overflow(int c) {
	waddch(log_win, c);
	wrefresh(log_win);
	// TODO also write to log file
	return c;
}


ncurses_stream::ncurses_stream(std::ostream &o) : std::ostream(&tbuf_), src_(o), srcbuf_(o.rdbuf()) {

	o.rdbuf(rdbuf());
}

ncurses_stream::~ncurses_stream() {
	src_.rdbuf(srcbuf_);
}
