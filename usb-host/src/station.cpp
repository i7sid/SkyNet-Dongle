/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#include "station.h"
#include "string_helper.h"
using namespace std;

unordered_map<string, station> stations;


// constructors
station::station() : mac("") {}
station::station(std::string mac) : mac(mac) {}


// setters

void station::set_mac(string mac) {
    this->mac = mac;
}
void station::set_position(double latitude, double longitude) {
    this->latitude = latitude;
    this->longitude = longitude;
}

void station::set_latitude(double latitude) {
    this->latitude = latitude;
}
void station::set_longitude(double longitude) {
    this->longitude = longitude;
}

void station::set_wind_speed(float value) {
    this->current_speed = value;
    // TODO fill history
}
void station::set_wind_direction(float value) {
    this->current_dir = value;
    // TODO fill history
}
void station::set_wind_direction_raw(float value) {
    this->current_dir_raw = value;
}
void station::set_compass(float value) {
    this->current_compass = value;
}
void station::update_position(string value) {
    this->pos_string = value;

    // split string up
    vector<string> parts = split(value.c_str(), ':');
    this->latitude = stod(parts[1]);
    if (parts[0].at(0) == 'S') this->latitude = -this->latitude;
    this->longitude = stod(parts[3]);
    if (parts[2].at(0) == 'S') this->longitude = -this->longitude;
}

// getters
string station::get_mac(void) const {
    return mac;
}
void station::get_position(double &latitude, double &longitude) const {
    latitude = this->latitude;
    longitude = this->longitude;
}
double station::get_latitude(void) const {
    return latitude;
}
double station::get_longitude(void) const {
    return longitude;
}
float station::get_wind_speed(void) const {
    return current_speed;
}
float station::get_wind_direction(void) const {
    return current_dir;
}
float station::get_wind_direction_raw(void) const {
    return current_dir_raw;
}
float station::get_compass(void) const {
    return current_compass;
}
string station::get_position_string(void) const {
    return pos_string;
}


std::ostream& operator<< (std::ostream &out, const station &s) {
    out << "Station: " << s.get_mac() << endl
        << s.get_position_string() << endl
        << std::setprecision(16)
        << s.get_latitude() << " | "
        << std::setprecision(16)
        << s.get_longitude() << endl
        << std::setprecision(5)
        << "Compass: " << s.get_compass() << "°" << endl
        << "Wind: " << s.get_wind_speed() << " km/h | "
        << s.get_wind_direction() << "°" << endl;
    return out;
}



// utility stuff, map management

int i = 0;
station& get_station(std::string mac) {
    station &s = stations[mac];
    s.set_wind_direction(i++);
    s.set_mac(mac);
    return stations[mac];
}


void list_stations(std::ostream &out) {
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        out << it->second << endl << endl;
    }
}

