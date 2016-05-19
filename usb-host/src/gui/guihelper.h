/**
 * @file
 *
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-05-17
 *
 * Copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

#ifndef GUIHELPER_H
#define GUIHELPER_H

#include <iostream>
#include <string>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

typedef struct start_base_t {
    std::string     mac;
    std::string     time_wind;
    std::string     time_pos;
} start_base_t;

start_base_t gui_dialog_base_start(std::string title, std::string mac, std::string time_wind, std::string time_pos);
void print_in_middle(WINDOW *win, int starty, int startx, int width, std::string s, chtype color);
std::string gui_dialog_mac(std::string title, std::string mac);

#endif /* !GUIHELPER_H */
