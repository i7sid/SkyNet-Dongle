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
#include <form.h>

using namespace std;

#ifndef NO_TAP
extern tap* ptr_tap;
#endif // NO_TAP

extern usb_tty* ptr_tty;
extern gui gui;
extern int local_mac[6];

int menu_item_count = 0;

WINDOW *log_win = nullptr;
WINDOW *my_form_win = nullptr;

static int menu_width = 36;
static int status_height = 12;

void clear_log(void);
void restart_dongle(void);
void goto_bootloader(void);
void set_mac_address(void);
void get_mac_address(void);
void set_ip_address(void);
void get_ip_address(void);
void calib_compass(void);
void test_device(void);
void exit_programm(void);

gui::gui() {
	int i = 0;
	items[i].name = "Fenster neu zeichnen";
	items[i++].func = clear_log;

	items[i].name = "   ";
	items[i++].func = nullptr;

	items[i].name = "Dongle neustarten";
	items[i++].func = restart_dongle;

	items[i].name = "Firmwareupdate starten";
	items[i++].func = goto_bootloader;

	items[i].name = "   ";
	items[i++].func = nullptr;

	items[i].name = "MAC-Adresse anzeigen";
	items[i++].func = get_mac_address;

	items[i].name = "MAC-Adresse setzen";
	items[i++].func = set_mac_address;

	items[i].name = "   ";
	items[i++].func = nullptr;

	items[i].name = "Geraet testen...";
	items[i++].func = test_device;

	items[i].name = "Kompass kalibrieren...";
	items[i++].func = calib_compass;

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
	int i = 1;
	werase(status_win);
	box(status_win, 0 , 0);
	mvwprintw(status_win, i++, 2, "Position:");
	mvwprintw(status_win, i++, 2, "%s", val_pos.c_str());
	mvwprintw(status_win, i++, 2, " ");
	mvwprintw(status_win, i++, 2, "Kompass:      %s", val_compass.c_str());
	mvwprintw(status_win, i++, 2, "Windrichtung: %s", val_winddir.c_str());
	mvwprintw(status_win, i++, 2, "Windgeschw.:  %s", val_windspeed.c_str());
	wrefresh(status_win);
}

void gui::update_size(void) {
    //cout << LINES << "x" << COLS << endl;
    clear();
//    resize_term(LINES, COLS);

    wresize(log_border_win, LINES, COLS - menu_width);
    box(log_border_win, 0 , 0);
    wrefresh(log_border_win);

    // left log window
    wresize(log_win, LINES - 2, COLS - menu_width - 2);
    wrefresh(log_win);

    // status window
    wmove(status_win, LINES - status_height, COLS - menu_width);
    wresize(status_win, status_height, menu_width);
	box(status_win, 0 , 0);
	update_status_win();

    // refresh menu
    wrefresh(cmd_win);

    redraw();
    ///cout << "lll" << endl;
    refresh();
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

void print_in_middle(WINDOW *win, int starty, int startx, int width, string s, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = s.length();
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", s.c_str());
	wattroff(win, color);
	refresh();
}

void gui::redraw() {
	redrawwin(log_win);
	redrawwin(status_win);
	redrawwin(cmd_win);
}

void clear_log(void) {
	gui.redraw();

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
	usb_message m;
	char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
	m.type = USB_CONTROL;
	m.payload_length = 1;
	m.payload = payload;
	m.payload[0] = (char)USB_CTRL_GET_MAC_ADDR;

	ptr_tty->usbSendMessage(m);
}
void set_ip_address(void) {
	usb_message m;
	char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
	m.type = USB_CONTROL;
	m.payload_length = 1;
	m.payload = payload;
	m.payload[0] = (char)USB_CTRL_GET_IP_ADDR;
	ptr_tty->usbSendMessage(m);
}
void get_ip_address(void) {
	usb_message m;
	char *payload = new char[USB_MAX_PAYLOAD_LENGTH];
	m.type = USB_CONTROL;
	m.payload_length = 1;
	m.payload = payload;
	m.payload[0] = (char)USB_CTRL_GET_MAC_ADDR;

	ptr_tty->usbSendMessage(m);
}

void calib_compass(void) {
	FIELD *field[3];
	FORM  *my_form;
	int ch, rows, cols;

	// Initialize the fields
	int i = 0;
	field[i++] = new_field(1, 28, 4, 2, 0, 0);
	//field[i++] = new_field(1, 10, 6, 18, 0, 0);
	field[i++] = NULL;

	set_field_back(field[0], A_UNDERLINE);
	field_opts_off(field[0], O_AUTOSKIP); /* Don't go to next field when this */
						  /* Field is filled up 		*/
	//set_field_back(field[1], A_UNDERLINE);
	//field_opts_off(field[1], O_AUTOSKIP);

	curs_set(TRUE);

	/* Create the form and post it */
	my_form = new_form(field);

	/* Calculate the area required for the form */
	scale_form(my_form, &rows, &cols);

	/* Create the window to be associated with the form */
	my_form_win = newwin(rows + 4, cols + 4, 4, 4);
	keypad(my_form_win, TRUE);

	/* Set main window and sub window */
	set_form_win(my_form, my_form_win);
	set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));

	/* Print a border around the main window and print a title */
	box(my_form_win, 0, 0);
	print_in_middle(my_form_win, 1, 0, cols + 4, "Kompass kalibrieren", COLOR_PAIR(2));

	post_form(my_form);
	wrefresh(my_form_win);

	set_current_field(my_form, field[0]);
	mvwprintw(my_form_win, 3, 2, "MAC address (2 Bytes  AB:CD) :");
	//mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
	wrefresh(my_form_win);

	string mac;
	bool loop = true;

	/* Loop through to get user requests */
	while (loop)
	{
		ch = getch();
		switch(ch)
		{
			case KEY_DOWN:
				/* Go to next field */
				form_driver(my_form, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(my_form, REQ_PREV_FIELD);
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_BACKSPACE:
			case KEY_DC:
			case 127:
				form_driver(my_form,REQ_LEFT_CHAR);
				form_driver(my_form,REQ_DEL_CHAR);
				break;
			case 27:
				loop = false;
				break;
			case 10:
			case KEY_ENTER:
				loop = false;
				form_driver(my_form, REQ_VALIDATION);
				mac = string(field_buffer(field[0], 0));
				break;

			default:
				/* If this is a normal character, it gets */
				/* Printed				  */
				form_driver(my_form, ch);
				break;
		}
		wrefresh(my_form_win);
		redrawwin(my_form_win);
	}

	/* Un post form and free the memory */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]);
	redrawwin(log_win);
	wrefresh(log_win);
	delwin(my_form_win);
	my_form_win = nullptr;

	if (mac.length() > 0) {
		COLOR_DBG();
		cerr << endl << "Try to start calibration routine on " << mac << endl;
		COLOR_RESET();

        int values[2];
        if (mac.length() >= 5 &&
            2 == sscanf(mac.c_str(), "%x:%x",
                        &values[0], &values[1])) {

                char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
                mac_frame_data frame;
                mac_frame_data_init(&frame);

                frame.payload = NULL;
                frame.payload_size = 0;

                MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                frame.mhr.dest_pan_id[0] = 0;
                frame.mhr.dest_pan_id[1] = 0;
                frame.mhr.dest_address[0] = values[0];
                frame.mhr.dest_address[1] = values[1];

                MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                frame.mhr.src_pan_id[0] = 0;
                frame.mhr.src_pan_id[1] = 0;
                frame.mhr.src_address[0] = (local_mac[4] & 0xFF);
                frame.mhr.src_address[1] = (local_mac[5] & 0xFF);


                // generate extended headers
                mac_extheader hdr;
                mac_extheader_init(&hdr);
                hdr.typelength_union.type_length.length = 1;
                hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD;
                hdr.data[0] = dongle_command::CALIB_COMPASS;

                frame.extheader = &hdr;

                uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

                usb_message m;
                m.type = USB_SKYNET_PACKET;
                m.payload_length = usb_length;
                m.payload = usb_payload;
                ptr_tty->usbSendMessage(m);

                COLOR_DBG();
                cerr << endl << "Press enter to stop calibration" << endl;
                COLOR_RESET();

                loop = true;
                while (loop) {
                	ch = getch();
                	if (ch == 10 || ch == KEY_ENTER) {
                		loop = false;
                	}
                }

                // try to stop calibration
                usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
				mac_frame_data_init(&frame);

				frame.payload = NULL;
				frame.payload_size = 0;

				MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
				frame.mhr.dest_pan_id[0] = 0;
				frame.mhr.dest_pan_id[1] = 0;
				frame.mhr.dest_address[0] = values[0];
				frame.mhr.dest_address[1] = values[1];

				MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
				frame.mhr.src_pan_id[0] = 0;
				frame.mhr.src_pan_id[1] = 0;
				frame.mhr.src_address[0] = (local_mac[4] & 0xFF);
				frame.mhr.src_address[1] = (local_mac[5] & 0xFF);


				// generate extended headers
				mac_extheader_init(&hdr);
				hdr.typelength_union.type_length.length = 1;
				hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD;
				hdr.data[0] = dongle_command::CALIB_COMPASS_STOP;

				frame.extheader = &hdr;

				usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

				m.type = USB_SKYNET_PACKET;
				m.payload_length = usb_length;
				m.payload = usb_payload;
				ptr_tty->usbSendMessage(m);

        }
        else {
        	COLOR_ERR();
            cerr << "MAC address malformed. Please use format   AA:BB ." << endl;
            COLOR_RESET();
        }

	}

}

void test_device(void) {
	FIELD *field[3];
	FORM  *my_form;
	WINDOW *my_form_win;
	int ch, rows, cols;

	// Initialize the fields
	int i = 0;
	field[i++] = new_field(1, 28, 4, 2, 0, 0);
	//field[i++] = new_field(1, 10, 6, 18, 0, 0);
	field[i++] = NULL;

	set_field_back(field[0], A_UNDERLINE);
	field_opts_off(field[0], O_AUTOSKIP); /* Don't go to next field when this */
						  /* Field is filled up 		*/
	//set_field_back(field[1], A_UNDERLINE);
	//field_opts_off(field[1], O_AUTOSKIP);

	curs_set(TRUE);

	/* Create the form and post it */
	my_form = new_form(field);

	/* Calculate the area required for the form */
	scale_form(my_form, &rows, &cols);

	/* Create the window to be associated with the form */
	my_form_win = newwin(rows + 4, cols + 4, 4, 4);
	keypad(my_form_win, TRUE);

	/* Set main window and sub window */
	set_form_win(my_form, my_form_win);
	set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));

	/* Print a border around the main window and print a title */
	box(my_form_win, 0, 0);
	print_in_middle(my_form_win, 1, 0, cols + 4, "Kompass kalibrieren", COLOR_PAIR(2));

	post_form(my_form);
	wrefresh(my_form_win);

	set_current_field(my_form, field[0]);
	mvwprintw(my_form_win, 3, 2, "MAC address (2 Bytes  AB:CD) :");
	//mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
	wrefresh(my_form_win);

	string mac;
	bool loop = true;

	/* Loop through to get user requests */
	while (loop)
	{
		ch = getch();
		switch(ch)
		{
			case KEY_DOWN:
				/* Go to next field */
				form_driver(my_form, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(my_form, REQ_PREV_FIELD);
				form_driver(my_form, REQ_END_LINE);
				break;
			case KEY_BACKSPACE:
			case KEY_DC:
			case 127:
				form_driver(my_form,REQ_LEFT_CHAR);
				form_driver(my_form,REQ_DEL_CHAR);
				break;
			case 27:
				loop = false;
				break;
			case 10:
			case KEY_ENTER:
				loop = false;
				form_driver(my_form, REQ_VALIDATION);
				mac = string(field_buffer(field[0], 0));
				break;

			default:
				/* If this is a normal character, it gets */
				/* Printed				  */
				form_driver(my_form, ch);
				break;
		}
		wrefresh(my_form_win);
		redrawwin(my_form_win);
	}

	/* Un post form and free the memory */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]);
	redrawwin(log_win);
	wrefresh(log_win);
	delwin(my_form_win);
	my_form_win = nullptr;

	if (mac.length() > 0) {
		COLOR_DBG();
		cerr << endl << "Try to ping device  " << mac << endl;
		COLOR_RESET();

        int values[2];
        if (mac.length() >= 5 &&
            2 == sscanf(mac.c_str(), "%x:%x",
                        &values[0], &values[1])) {

                char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
                mac_frame_data frame;
                mac_frame_data_init(&frame);

                // can be used to DEBUG  RF (be reminded about overhead by header: 16 Byte)
                frame.payload = (uint8_t*)string("..........abcdefghijklmnopqrstABCDEFGHIJKLMNOPQRST----------a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,A,B,C,D,E,F,G,H,I,J,K,L,MNOPQRST").c_str();
                frame.payload_size = 0;

                MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                frame.mhr.dest_pan_id[0] = 0;
                frame.mhr.dest_pan_id[1] = 0;
                frame.mhr.dest_address[0] = values[0];
                frame.mhr.dest_address[1] = values[1];

                MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
                frame.mhr.src_pan_id[0] = 0;
                frame.mhr.src_pan_id[1] = 0;
                frame.mhr.src_address[0] = (local_mac[4] & 0xFF);
                frame.mhr.src_address[1] = (local_mac[5] & 0xFF);


                // generate extended headers
                mac_extheader hdr;
                mac_extheader_init(&hdr);
                hdr.typelength_union.type_length.length = 1;
                hdr.typelength_union.type_length.type = EXTHDR_DONGLE_CMD;
                hdr.data[0] = dongle_command::TEST;

                frame.extheader = &hdr;

                uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

                usb_message m;
                m.type = USB_SKYNET_PACKET;
                m.payload_length = usb_length;
                m.payload = usb_payload;
                ptr_tty->usbSendMessage(m);
        }
        else {
        	COLOR_ERR();
            cerr << endl << "MAC address malformed. Please use format   AA:BB ." << endl;
            COLOR_RESET();
        }
	}
}

void exit_programm(void) {
	exit(0);
}
