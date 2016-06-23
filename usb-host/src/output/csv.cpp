/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#include <cmath>
#include <iostream>
#include <iomanip>
#include "station.h"
#include "csv.h"

using namespace std;



CsvOutput::CsvOutput() : filename("") {}

CsvOutput::CsvOutput(std::string filename) : filename(filename) {}
void CsvOutput::set_filename(std::string filename) {
    this->filename = filename;
}

void CsvOutput::updated(void) {
    ofstream of;
    ofstream of_pos;
	of.open("wind-" + filename, std::ofstream::out | std::ofstream::app);
	of_pos.open("pos-" + filename, std::ofstream::out | std::ofstream::app);

    for (auto it = stations.begin(); it != stations.end(); ++it) {
        if (last_wind_time.compare(it->second.get_last_wind_time()) != 0) {
            of << it->second.get_last_wind_time() << ","
                << it->second.get_mac() << ","
                << std::setprecision(5)
                << it->second.get_wind_speed() << ","
                << it->second.get_wind_direction() << endl;
            last_wind_time = it->second.get_last_wind_time();
        }
        if (this->last_pos_time.compare(it->second.get_last_pos_time()) != 0) {
            of_pos << it->second.get_last_pos_time() << ","
                << it->second.get_mac() << ","
                << std::setprecision(10)
                << it->second.get_longitude() << ","
                << it->second.get_latitude() << endl;
            this->last_pos_time = it->second.get_last_pos_time();
        }
    }

    of.close();
    of_pos.close();
}
