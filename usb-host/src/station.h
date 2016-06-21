/**
 * @file
 *
 * @brief   Management of stations and their current measurement data.
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#ifndef STATION_H
#define STATION_H

#include <string>
#include <iostream>
#include <iomanip>
#include <unordered_map>

#define FILTER_LONG_WINDOW      (180)
#define FILTER_SHORT_WINDOW     (42)


class station {
    public:
        station(void);
        station(std::string mac);

        // setters
        void    set_mac(std::string mac);
        void    set_position(double latitude, double longitude);

        void    set_latitude(double latitude);
        void    set_longitude(double longitude);

        void    set_wind_speed(float value);
        void    set_wind_direction(float value);
        void    set_wind_direction_raw(float value);
        void    set_compass(float value);
        void    update_position(std::string);

        // getters
        std::string get_mac(void) const;
        void    get_position(double &latitude, double &longitude) const;
        double  get_latitude(void) const;
        double  get_longitude(void) const;
        float   get_wind_speed(void) const;
        float   get_wind_direction(void) const;
        float   get_wind_direction_raw(void) const;
        float   get_compass(void) const;
        std::string get_position_string(void) const;

        friend std::ostream& operator<< (std::ostream &out, const station &s);

    protected:
        std::string mac;
        std::string pos_string  = "";
        double  latitude        = 0;
        double  longitude       = 0;
        float   current_speed   = 0;
        float   current_dir     = 0;
        float   current_dir_raw = 0;
        float   current_compass = 0;
        float   historic_speed[FILTER_LONG_WINDOW];
        float   historic_dir[FILTER_LONG_WINDOW];
};

extern std::unordered_map<std::string, station> stations;

station& get_station(std::string mac);

void list_stations(std::ostream &out);


#endif /* !STATION_H */
