/**
 * @file
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-09-05
 */

#include "thermal.h"


float thermal::calc_updraft_score(void) const {
    float s = 0;
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        s += it->second.get_base_hist_speed_diff();
        s += abs(it->second.get_base_hist_dir_diff()) * 0.1F;
    }

    return s;
}


bool thermal::calc_updraft_position(station &s1, station &s2) {
    double tan1 = tan(deg2rad(s1.get_base_hist_dir_short()));
    double tan2 = tan(deg2rad(s2.get_base_hist_dir_short()));

    double x1;
    double y1;
    double x2;
    double y2;

    s1.get_position(y1, x1);
    s2.get_position(y2, x2);

    if (tan1 == tan2) return false;

    double t2 = (x1 - x2 + (y2 - y1) * tan1) / (tan2 - tan1);
    double t1 = y2 + t2 - y1;

    // updraft center would be behind station (against the wind)
    if (t2 < 0 || t1 < 0) return false;

    updraft_pos_lon = x2 + t2 * tan2;
    updraft_pos_lat = y2 + t2;

    return true;
}

void thermal::calc_updraft_offset(double height, station &s1, station &s2) {
    if (updraft_pos_lon == 0 || updraft_pos_lat == 0) return;

    double v_long = (s1.get_base_hist_speed_long() + s1.get_base_hist_speed_long()) / 2.0;
    double dir_long = (s1.get_base_hist_dir_long() + s1.get_base_hist_dir_long()) / 2.0;

    double t = height / UPDRAFT_SPEED;
    double rad = deg2rad(dir_long);

    double vu_lon = v_long * sin(rad);
    double vu_lat = v_long * cos(rad);

    updraft_pos_air_lon     = updraft_pos_lon + meters2degrees(t * vu_lon);
    updraft_pos_air_lat     = updraft_pos_lat + meters2degrees(t * vu_lat);
    updraft_pos_air_height  = height;
}

inline double thermal::meters2degrees(double m) const {
    return (m / 111111.0);
}

double thermal::get_updraft_latitude(void) const {
    return updraft_pos_lat;
}

double thermal::get_updraft_longitude(void) const {
    return updraft_pos_lon;
}
