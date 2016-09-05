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

    double x1; // y1
    double y1; // x1
    double x2; // y2
    double y2; // x2

    s1.get_position(y1, x1);
    s2.get_position(y2, x2);

    if (tan1 == tan2) return false;

    double t2 = (x1 - x2 + (y2 - y1) * tan1) / (tan2 - tan1);

    updraft_pos_lon = x2 + t2 * tan2;
    updraft_pos_lat = y2 + t2;

    return true;
}

double thermal::get_updraft_latitude(void) const {
    return updraft_pos_lat;
}

double thermal::get_updraft_longitude(void) const {
    return updraft_pos_lon;
}
