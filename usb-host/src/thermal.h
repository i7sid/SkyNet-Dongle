/**
 * @file
 *
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-09-05
 */

#ifndef THERMAL_H
#define THERMAL_H

#include <cmath>
#include "station.h"

#define PI                  (3.14159265)
#define UPDRAFT_SPEED       (2.0)

class thermal {
    public:
        /**
         * @brief   Calculates the probability that an updraft occurred recently.
         * @return  A score with zero = neutral, -Inf = no updraft, +Inf = updraft!
         */
        float calc_updraft_score(void) const;

        /**
         * @brief   Calculates the updraft position (if any) on earth between two stations.
         * @return  True if calculated, false otherwise.
         */
        bool calc_updraft_position(station &s1, station &s2);

        void calc_updraft_offset(double height, station &s1, station &s2);

        double get_updraft_latitude(void) const;
        double get_updraft_longitude(void) const;


    protected:
        double meters2degrees(double m) const;

        double updraft_pos_lat          = 0;
        double updraft_pos_lon          = 0;
        double updraft_pos_air_lat      = 0;
        double updraft_pos_air_lon      = 0;
        double updraft_pos_air_height   = 0;
};

inline double deg2rad(double deg) {
    return (deg * PI) / 180.0;
}

extern thermal therm;

#endif /* !THERMAL_H */
