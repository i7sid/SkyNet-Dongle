/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#ifndef NO_DB
#include "output/output.h"
#include "output/db.h"
#include "db/db.h"
#include "station.h"

using namespace std;

DbOutput::DbOutput() :
    dbcon("localhost", "skynetbaseweb", "skynetbaseweb", "skynetbaseweb") {}


void DbOutput::updated(void) {

    for (auto it = stations.begin(); it != stations.end(); ++it) {
		int station = dbcon.get_station(it->second.get_mac());
        stringstream ss;
        ss << setprecision(10);

        if (last_wind_time.compare(it->second.get_last_wind_time()) != 0) {
            ss << it->second.get_wind_speed();
            dbcon.record_entity(station, DB_TYPE_WIND_SPEED, it->second.get_last_wind_time(), ss.str());
            ss.str("");
            ss << it->second.get_wind_direction();
            dbcon.record_entity(station, DB_TYPE_WIND_DIR_COMP, it->second.get_last_wind_time(), ss.str());
            ss.str("");
            ss << it->second.get_wind_direction_raw();
            dbcon.record_entity(station, DB_TYPE_WIND_DIR_RAW, it->second.get_last_wind_time(), ss.str());

            last_wind_time = it->second.get_last_wind_time();
        }
        if (this->last_pos_time.compare(it->second.get_last_pos_time()) != 0) {
            ss.str("");
            ss << it->second.get_position_string();
            dbcon.record_entity(station, DB_TYPE_GPS, it->second.get_last_pos_time(), ss.str());
            ss.str("");
            ss << it->second.get_compass();
            dbcon.record_entity(station, DB_TYPE_COMPASS, it->second.get_last_pos_time(), ss.str());

            this->last_pos_time = it->second.get_last_pos_time();
        }
    }

}

#endif
