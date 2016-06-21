/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#include "station.h"
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
void station::set_compass(float value) {
    this->current_compass = value;
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
float station::get_compass(void) const {
    return current_compass;
}


std::ostream& operator<< (std::ostream &out, const station &s) {
    out << s.get_mac() << endl
        << s.get_latitude() << " | " << s.get_longitude()
        << s.get_compass() << "°" << endl
        << s.get_wind_speed() << " km/h | "
        << s.get_wind_direction() << "°" << endl;
    return out;
}



// utility stuff, map management

station& get_station(std::string mac) {
    station &s = stations[mac];
    s.set_mac(mac);
    return s;
}


void list_stations(std::ostream &out) {
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        out << it->second << endl << endl;
    }
}

