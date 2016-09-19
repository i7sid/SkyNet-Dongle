/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#include <cmath>
#include <iomanip>
#include "station.h"
#include "json.h"

using namespace std;

static inline double deg2rad(double input) { return input * (M_PI / 180); }


JsonOutput::JsonOutput(std::string filename) : filename(filename) {
}

void JsonOutput::updated(station*) {
    ofstream of_json;
	of_json.open(filename, std::ofstream::out | std::ofstream::trunc);
    //list_stations(of_json);

    of_json << "{ \"type\": \"FeatureCollection\"," << endl;
    of_json << "\"features\": [ " << endl;
    
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        of_json << "        { \"type\": \"Feature\"," << endl;
        of_json << "          \"geometry\": {" << endl;
        of_json << "            \"type\": \"Point\"," << endl;
        of_json << "            \"coordinates\": [" << endl;
        of_json << std::setprecision(16);
        of_json << "              " << it->second.get_longitude()
                                    << ", " << it->second.get_latitude() << endl;
        of_json << "              ]" << endl;
        of_json << "            }," << endl;
        of_json << "          \"properties\": {" << endl;
        of_json << "            \"type\": \"station\"," << endl;
        of_json << "            \"name\": \"" << it->second.get_mac() << "\"" << endl;
        of_json << "            }" << endl;
        of_json << "          }," << endl;

        // now calculate wind signs
        // (this distance calculations work only for smaller distances
        //  as they omit the spherical form factor of earth)

        double r = it->second.get_wind_speed() * 0.0150;     // in km
        int alpha = it->second.get_wind_direction();
        alpha = (360 - alpha + 90) % 360;
        double dx = r * cos(deg2rad(alpha));
        double dy = r * sin(deg2rad(alpha));

        // source: https://www.kompf.de/gps/distcalc.html
        double target_x = it->second.get_longitude() + (dx / (111.3 * cos(deg2rad(it->second.get_latitude()))));
        double target_y = it->second.get_latitude() + (dy / 111.3);

        of_json << "        { \"type\": \"Feature\"," << endl;
        of_json << "          \"geometry\": {" << endl;
        of_json << "            \"type\": \"LineString\"," << endl;
        of_json << "            \"coordinates\": " << endl <<
                                    "[" << endl;
        of_json << std::setprecision(16);
        of_json << "                   [" << it->second.get_longitude()
                                       << ", " << it->second.get_latitude() << endl;
        of_json << "                   ], [" << target_x
                                       << ", " << target_y << endl;
        of_json << "                   ]" << endl;
        of_json << "                 ]" << endl;
        of_json << "            }," << endl;
        of_json << "          \"properties\": {" << endl;
        of_json << "            \"type\": \"wind\"," << endl;
        of_json << "            \"name\": \"current_" << it->second.get_mac() << "\"" << endl;
        of_json << "            }" << endl;
        of_json << "          }," << endl;


    }

    of_json << " {}" << endl;
    of_json << " ]" << endl;
    of_json << "}" << endl;

    of_json << endl;
    of_json.close();
}
