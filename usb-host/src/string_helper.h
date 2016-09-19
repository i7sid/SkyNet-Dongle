/**
 * @file
 *
 * @brief   some string helper functions
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 *
 * Copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

/// @brief trim from start
std::string &ltrim(std::string &s);

/// @brief trim from end
std::string &rtrim(std::string &s);

/// @brief trim from both ends
std::string &trim(std::string &s);

/// @brief split string up using given delimitors
std::vector<std::string> split(const char *str, char c);



#endif /* !STRING_HELPER_H */
