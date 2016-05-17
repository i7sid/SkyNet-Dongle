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

void print_in_middle(WINDOW *win, int starty, int startx, int width, std::string s, chtype color);
std::string gui_dialog_mac(std::string);

#endif /* !GUIHELPER_H */
