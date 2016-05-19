/**
 * @file
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <%MAIL>
 * @date    2016-05-17
 *
 * Copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

#include "guihelper.h"

using namespace std;

WINDOW *my_form_win = nullptr;
extern WINDOW *log_win;


start_base_t gui_dialog_base_start(string title, string mac, string time_wind, string time_pos) {
	FIELD *field[4];
	FORM  *my_form;
	int ch, rows, cols;

	// Initialize the fields
	int i = 0;
	field[i++] = new_field(1, 28, 3, 1, 0, 0);
	field[i++] = new_field(1, 10, 5, 20, 0, 0);
	field[i++] = new_field(1, 10, 6, 20, 0, 0);
	field[i++] = NULL;

	set_field_back(field[0], A_UNDERLINE);
	field_opts_off(field[0], O_AUTOSKIP); 
	set_field_back(field[1], A_UNDERLINE);
	field_opts_off(field[1], O_AUTOSKIP); 
	set_field_back(field[2], A_UNDERLINE);
	field_opts_off(field[2], O_AUTOSKIP); 

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
	print_in_middle(my_form_win, 1, 0, cols + 4, title.c_str(), COLOR_PAIR(2));


	post_form(my_form);
	wrefresh(my_form_win);

	set_current_field(my_form, field[0]);
	mvwprintw(my_form_win, 3, 2, "MAC address (2 Bytes  AB:CD) :");
	mvwprintw(my_form_win, 7, 2, "Delay Wind (ms):");
	mvwprintw(my_form_win, 8, 2, "Delay Position (ms):");

    // insert default value
    set_field_buffer(field[0], 0, mac.c_str());
    set_field_buffer(field[1], 0, time_wind.c_str());
    set_field_buffer(field[2], 0, time_pos.c_str());
	
    wrefresh(my_form_win);
    refresh();

	bool loop = true;

    // return data
    start_base_t r;

	/* Loop through to get user requests */
	while (loop)
	{
		ch = getch();
		switch(ch)
		{
            case '\t':
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
            case KEY_LEFT:
                form_driver(my_form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(my_form, REQ_NEXT_CHAR);
                break;
            case KEY_END:
                form_driver(my_form, REQ_END_LINE);
                break;
            case KEY_HOME:
                form_driver(my_form, REQ_BEG_LINE);
                break;
			case KEY_BACKSPACE:
			case 127:
				form_driver(my_form,REQ_LEFT_CHAR);
			case KEY_DC:
				form_driver(my_form,REQ_DEL_CHAR);
				break;
			case 27:
				loop = false;
				break;
			case 10:
			case KEY_ENTER:
				loop = false;
				form_driver(my_form, REQ_VALIDATION);
				r.mac = string(field_buffer(field[0], 0));
				r.time_wind = string(field_buffer(field[1], 0));
				r.time_pos = string(field_buffer(field[2], 0));
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

    return r;

}


string gui_dialog_mac(string title, string preset) {
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
	print_in_middle(my_form_win, 1, 0, cols + 4, title.c_str(), COLOR_PAIR(2));

	post_form(my_form);
	wrefresh(my_form_win);

	set_current_field(my_form, field[0]);
	mvwprintw(my_form_win, 3, 2, "MAC address (2 Bytes  AB:CD) :");
	//mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");

    // insert default value
    set_field_buffer(field[0], 0, preset.c_str());
	
    wrefresh(my_form_win);
    refresh();

	string mac;
	bool loop = true;

	/* Loop through to get user requests */
	while (loop)
	{
		ch = getch();
		switch(ch)
		{
            case '\t':
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
            case KEY_LEFT:
                form_driver(my_form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(my_form, REQ_NEXT_CHAR);
                break;
            case KEY_END:
                form_driver(my_form, REQ_END_LINE);
                break;
            case KEY_HOME:
                form_driver(my_form, REQ_BEG_LINE);
                break;
			case KEY_BACKSPACE:
			case 127:
				form_driver(my_form,REQ_LEFT_CHAR);
			case KEY_DC:
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

    return mac;

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
