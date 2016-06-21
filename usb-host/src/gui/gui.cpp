/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "gui.h"
#include "tap.h"
#include "usbtty.h"
#include "guihelper.h"
#include "station.h"
#include <usb/message.h>
#include <mac/mac.h>
#include <thread>         // std::thread (C++11!)
#include <sstream>

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

static int menu_width = 36;
static int status_height = 24;

static uint8_t seq_no = 0;

std::mutex log_mtx;           // mutex for critical log output section

void clear_log(void);
void restart_dongle(void);
void goto_bootloader(void);
void set_mac_address(void);
void get_mac_address(void);
void set_ip_address(void);
void get_ip_address(void);
void calib_compass(void);
void calib_compass_stop(void);
void test_device(void);
void start_base(void);
void stop_base(void);
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

	items[i].name = "Kompasskalib. stoppen...";
	items[i++].func = calib_compass_stop;

	items[i].name = "   ";
	items[i++].func = nullptr;

	items[i].name = "Base: Starte Messung";
	items[i++].func = start_base;

	items[i].name = "Base: Stoppe Messung";
	items[i++].func = stop_base;

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
    stringstream ss;
    list_stations(ss);

	int i = 1;
    for (std::string line; std::getline(ss, line);)
    {
        mvwprintw(status_win, i++, 2, line.c_str());
    }
	wrefresh(status_win);
}

void gui::update_size(void) {
    //cout << LINES << "x" << COLS << endl;
    //clear();
    //resize_term(LINES, COLS);

    /*
    clear();
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    */

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
	bool loop = true;
    int ch;
    string mac = gui_dialog_mac("Kompass kalibrieren", "FF:FF");

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
                frame.mhr.seq_no = seq_no++;

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
                frame.mhr.seq_no = seq_no++;

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

                COLOR_DBG();
                cerr << endl << "Request sent..." << endl;
                COLOR_RESET();

        }
        else {
        	COLOR_ERR();
            cerr << "MAC address malformed. Please use format   AA:BB ." << endl;
            COLOR_RESET();
        }
	}
}

void calib_compass_stop(void) {
    string mac = gui_dialog_mac("Kompass kalibrieren", "FF:FF");

	if (mac.length() > 0) {
		COLOR_DBG();
		cerr << endl << "Try to stop calibration routine on " << mac << endl;
		COLOR_RESET();

        int values[2];
        if (mac.length() >= 5 &&
            2 == sscanf(mac.c_str(), "%x:%x",
                        &values[0], &values[1])) {

                // try to stop calibration
                char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
                mac_frame_data frame;
				mac_frame_data_init(&frame);

				frame.payload = NULL;
				frame.payload_size = 0;
                frame.mhr.seq_no = seq_no++;

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
				hdr.data[0] = dongle_command::CALIB_COMPASS_STOP;

				frame.extheader = &hdr;

				uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

                usb_message m;
				m.type = USB_SKYNET_PACKET;
				m.payload_length = usb_length;
				m.payload = usb_payload;
				ptr_tty->usbSendMessage(m);

                COLOR_DBG();
                cerr << endl << "Request sent..." << endl;
                COLOR_RESET();

        }
        else {
        	COLOR_ERR();
            cerr << "MAC address malformed. Please use format   AA:BB ." << endl;
            COLOR_RESET();
        }
	}
}


void test_device(void) {
    string mac = gui_dialog_mac("Gerät testen", "FF:FF");

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
                frame.mhr.seq_no = seq_no++;

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

                COLOR_DBG();
                cerr << endl << "Request sent..." << endl;
                COLOR_RESET();
        }
        else {
        	COLOR_ERR();
            cerr << endl << "MAC address malformed. Please use format   AA:BB ." << endl;
            COLOR_RESET();
        }
	}
}

void start_base(void) {
    start_base_t times = gui_dialog_base_start("Messung starten", "FF:FF",
                                                "5000", "61000");
    int values[2];
    if (times.mac.length() < 5 ||
        2 != sscanf(times.mac.c_str(), "%x:%x",
                    &values[0], &values[1])) {

        COLOR_ERR();
        cerr << endl << "MAC address malformed. Please use format   AA:BB ." << endl;
        COLOR_RESET();
        return;
    }
    try {
        int time_wind = stoi(times.time_wind);
        int time_pos = stoi(times.time_pos);
        cerr << time_wind << ": " << time_pos << endl;

        char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
        mac_frame_data frame;
        mac_frame_data_init(&frame);

        frame.payload = new uint8_t[8];
        frame.payload_size = 8;
        frame.mhr.seq_no = seq_no++;

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
        hdr.data[0] = dongle_command::BASE_START_SENDING;

        frame.extheader = &hdr;

        frame.payload[0] = ((time_wind     ) & 0xFF);
        frame.payload[1] = ((time_wind >>  8) & 0xFF);
        frame.payload[2] = ((time_wind >> 16) & 0xFF);
        frame.payload[3] = ((time_wind >> 24) & 0xFF);
        frame.payload[4] = ((time_pos      ) & 0xFF);
        frame.payload[5] = ((time_pos >>  8) & 0xFF);
        frame.payload[6] = ((time_pos >> 16) & 0xFF);
        frame.payload[7] = ((time_pos >> 24) & 0xFF);

        uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

        usb_message m;
        m.type = USB_SKYNET_PACKET;
        m.payload_length = usb_length;
        m.payload = usb_payload;
        ptr_tty->usbSendMessage(m);

        COLOR_DBG();
        cerr << endl << "Request sent..." << endl;
        COLOR_RESET();
    }
    catch (std::exception) {
        COLOR_ERR();
        cerr << endl << "Error. Malformed input?" << endl;
        COLOR_RESET();
    }
}

void stop_base(void) {
    string mac = gui_dialog_mac("Messung stoppen", "FF:FF");

    int values[2];
    if (mac.length() < 5 ||
        2 != sscanf(mac.c_str(), "%x:%x",
                    &values[0], &values[1])) {

        COLOR_ERR();
        cerr << endl << "MAC address malformed. Please use format   AA:BB ." << endl;
        COLOR_RESET();
        return;
    }

    char* usb_payload = new char[USB_MAX_PAYLOAD_LENGTH];
    mac_frame_data frame;
    mac_frame_data_init(&frame);

    frame.payload = NULL;
    frame.payload_size = 0;
    frame.mhr.seq_no = seq_no++;

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
    hdr.data[0] = dongle_command::BASE_STOP_SENDING;

    frame.extheader = &hdr;


    uint16_t usb_length = mac_frame_data_pack(&frame, (uint8_t*)usb_payload);

    usb_message m;
    m.type = USB_SKYNET_PACKET;
    m.payload_length = usb_length;
    m.payload = usb_payload;
    ptr_tty->usbSendMessage(m);

    COLOR_DBG();
    cerr << endl << "Request sent..." << endl;
    COLOR_RESET();

}

void exit_programm(void) {
	exit(0);
}
