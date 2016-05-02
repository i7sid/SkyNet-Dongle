/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "gui.h"
#include "tap.h"
#include "usbtty.h"
#include <usb/message.h>
#include <mac/mac.h>

#include <ncurses.h>
#include <menu.h>

using namespace std;

extern tap* ptr_tap;
extern usb_tty* ptr_tty;


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

WINDOW* log_win;

void restart_dongle(void);
void goto_bootloader(void);
void set_mac_address(void);
void get_mac_address(void);
void exit_programm(void);

gui::gui() {
	items[0].name = "Dongle neustarten";
	items[0].func = restart_dongle;
	items[1].name = "Firmwareupdate starten";
	items[1].func = goto_bootloader;
	items[2].name = "   ";
	items[2].func = nullptr;
	items[3].name = "MAC-Adresse anzeigen";
	items[3].func = get_mac_address;
	items[4].name = "MAC-Adresse setzen";
	items[4].func = set_mac_address;
	items[5].name = "   ";
	items[5].func = nullptr;
	items[6].name = "Beenden";
	items[6].func = exit_programm;
}
gui::~gui() {
    endwin();
//    fprintf(stderr, "Program terminated by SIGINT.\n");
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

    int menu_width = 36;

    ncurses_stream foo(std::cout);
    //ncurses_stream foo2(std::cerr);


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

    n_choices = ARRAY_SIZE(items);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

    for(i = 0; i < n_choices; ++i) {
        my_items[i] = new_item(items[i].name.c_str(), NULL); //, choices[i]);
        set_item_userptr(my_items[i], (void*)(items[i].func));
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
                ITEM *cur;
                void (*p)(void);

                cur = current_item(menu);
                p = (void (*)())item_userptr(cur);
                if (p != nullptr) p();

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



void restart_dongle(void) {
	COLOR_DBG();
	cout << "Sending command to restart dongle..." << endl;
	COLOR_RESET();

	usb_message m;
	char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
	m.type = USB_CONTROL;
	m.payload_length = 1;
	m.payload = payload;
	m.payload[0] = (char)USB_CTRL_RESET;

	ptr_tty->usbSendMessage(m);
}
void goto_bootloader(void) {
	COLOR_DBG();
	cout << "Sending command to go to bootloader..." << endl;
	COLOR_RESET();

	usb_message m;
	char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
	m.type = USB_CONTROL;
	m.payload_length = 1;
	m.payload = payload;
	m.payload[0] = (char)USB_CTRL_BOOTLOADER;

	ptr_tty->usbSendMessage(m);
}
void set_mac_address(void) {

}
void get_mac_address(void) {

}
void exit_programm(void) {
	exit(0);
}
