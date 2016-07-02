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

    // fill history
    if (history_speed.size() >= FILTER_LONG_WINDOW) {
        history_speed.pop_front();
    }
    history_speed.push_back(value);
}
void station::set_wind_direction(float value) {
    this->current_dir = value;

    // fill history
    if (history_dir.size() >= FILTER_LONG_WINDOW) {
        history_dir.pop_front();
    }
    history_dir.push_back(value);
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
    double src_lat = stod(parts[1]);
    double deg_lat = floor(src_lat / 100.0);
    double min_lat = src_lat - deg_lat * 100.0;
    this->latitude = deg_lat + min_lat / 60.0;

    double src_lon = stod(parts[3]);
    double deg_lon = floor(src_lon / 100.0);
    double min_lon = src_lon - deg_lon * 100.0;
    this->longitude = deg_lon + min_lon / 60.0;

    if (parts[0].at(0) == 'S') this->latitude = -this->latitude;
    if (parts[2].at(0) == 'S') this->longitude = -this->longitude;
}
void station::set_last_wind_time(std::string time) {
    last_wind_time = time;
}
void station::set_last_pos_time(std::string time) {
    last_pos_time = time;
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
string station::get_last_wind_time(void) const {
    return last_wind_time;
}
string station::get_last_pos_time(void) const {
    return last_pos_time;
}
float station::average_wind_speed(size_t window) const {
    size_t n = min(window, history_speed.size());
    if (n == 0) return 0;

    float sum = 0;
    for (size_t i = history_speed.size() - n; i < history_speed.size(); ++i) {
        sum += history_speed[i];
    }

    return (sum / static_cast<float>(n));
}
float station::average_wind_direction(size_t window) const {
    size_t n = min(window, history_dir.size());
    if (n == 0) return 0;

    float sum = 0;
    for (size_t i = history_dir.size() - n; i < history_dir.size(); ++i) {
        sum += history_dir[i];
    }

    return (sum / static_cast<float>(n));
}


std::ostream& operator<< (std::ostream &out, const station &s) {
    float speed_avg_long = s.average_wind_speed(FILTER_LONG_WINDOW);
    float speed_avg_short = s.average_wind_speed(FILTER_SHORT_WINDOW);

    out << "Station: " << s.get_mac() << endl
        << s.get_position_string() << endl
        << std::setprecision(10)
        << s.get_latitude() << " | "
        << std::setprecision(10)
        << s.get_longitude() << " | "
        << std::setprecision(5)
        << s.get_compass() << "°" << endl
        << "Wind: " << s.get_wind_speed() << " km/h | "
        << s.get_wind_direction() << "°" << endl
        << "Hist (S/L):    "
        << speed_avg_short << " km/h / "
        << speed_avg_long << " km/h" << endl
        << "HistD (S/now): "
        << speed_avg_short - speed_avg_long << " km/h / "
        << s.get_wind_speed() - speed_avg_long << " km/h" << endl
        ;
    return out;
}



// utility stuff, map management

int i = 0;
station& get_station(std::string mac) {
    station &s = stations[mac];
    s.set_mac(mac);
    return stations[mac];
}


void list_stations(std::ostream &out) {
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        out << it->second << endl << endl;
    }
}

