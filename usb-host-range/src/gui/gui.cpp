/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "gui.h"
#include "usbtty.h"
#include "guihelper.h"
#include <usb/message.h>
#include <thread>         // std::thread (C++11!)
#include <sstream>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

using namespace std;

extern gui gui;
extern int local_mac[6];

int menu_item_count = 0;

WINDOW *log_win = nullptr;

static int menu_width = 48;
static int status_height = 24;

std::mutex log_mtx;           // mutex for critical log output section

void clear_log(void);
void exit_programm(void);

gui::gui() {
	int i = 0;
	items[i].name = "Fenster neu zeichnen";
	items[i++].func = clear_log;

	items[i].name = "   ";
	items[i++].func = nullptr;

	items[i].name = "Beenden";
	items[i++].func = exit_programm;

	menu_item_count = i;
}
gui::~gui() {
    endwin();
//    fprintf(stderr, "Program terminated by SIGINT.\n");
}

void gui::update_status_win() {
    werase(status_win);
	box(status_win, 0 , 0);
    stringstream ss;
    /*
    list_stations(ss);

	int i = 1;
    for (std::string line; std::getline(ss, line);)
    {
        mvwprintw(status_win, i++, 2, line.c_str());
    }
    */

	wrefresh(status_win);
}

void gui::update_size(void) {
    clearok(curscr, TRUE);

    // left log window
    wresize(log_border_win, LINES, COLS - menu_width);
    wresize(log_win, LINES - 2, COLS - menu_width - 2);

    // status window
    wresize(status_win, status_height, menu_width);
    mvwin(status_win, LINES - status_height, COLS - menu_width);

    // menu win
    wresize(cmd_win, LINES - status_height, menu_width);
    mvwin(cmd_win, 0, COLS - menu_width);

    box(log_border_win, 0 , 0);
    box(cmd_win, 0 , 0);
	box(status_win, 0 , 0);
    /*
    wrefresh(cmd_win);
    wrefresh(log_border_win);
    wrefresh(log_win);
    */

    wnoutrefresh(cmd_win);
    wnoutrefresh(log_border_win);
    wnoutrefresh(log_win);
    doupdate();
	update_status_win();

    redraw();

    cout << endl;
}

void gui::init() {
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    refresh();

    ncurses_stream foo(std::cout);
    //ncurses_stream foo2(std::cerr);


    log_border_win = newwin(LINES, COLS - menu_width, 0, 0);
    box(log_border_win, 0 , 0);
    wrefresh(log_border_win);

    // left log window
    log_win = newwin(LINES - 2, COLS - menu_width - 2, 1, 1);
    scrollok(log_win, TRUE);
    wrefresh(log_win);
    refresh();

    // menu outer window
    cmd_win = newwin(LINES - status_height, menu_width, 0, COLS - menu_width);
    box(cmd_win, 0 , 0);
    wrefresh(cmd_win);
    keypad(cmd_win, TRUE);


    // status window
    status_win = newwin(status_height, menu_width, LINES - status_height, COLS - menu_width);
	box(status_win, 0 , 0);
	update_status_win();



    // menu
    ITEM **my_items;
    int n_choices, i;

    n_choices = menu_item_count;
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

    for(i = 0; i < n_choices; ++i) {
        my_items[i] = new_item(items[i].name.c_str(), NULL); //, choices[i]);
        set_item_userptr(my_items[i], (void*)(items[i].func));
    }
    my_items[n_choices] = (ITEM *)NULL;

    menu = new_menu((ITEM **)my_items);
    set_menu_win(menu, cmd_win);
    set_menu_sub(menu, derwin(cmd_win, LINES - status_height - 4, menu_width - 4, 2, 2));
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
                ITEM *cur;
                void (*p)(void);

                cur = current_item(menu);
                p = (void (*)())item_userptr(cur);
                if (p != nullptr) p();

                pos_menu_cursor(menu);
                break;
            }
            case KEY_RESIZE:
                update_size();
                break;
        }
        wrefresh(cmd_win);
        //refresh();
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


void gui::redraw() {
	redrawwin(log_win);
	redrawwin(status_win);
	redrawwin(log_border_win);
	redrawwin(cmd_win);
}

void clear_log(void) {
	gui.redraw();
}

void exit_programm(void) {
	exit(0);
}
