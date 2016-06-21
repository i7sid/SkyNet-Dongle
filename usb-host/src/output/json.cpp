/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#include <iomanip>
#include "station.h"
#include "json.h"

using namespace std;

JsonOutput::JsonOutput(std::string filename) : filename(filename) {
}

void JsonOutput::updated(void) {
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
        of_json << "            \"name\": \" " << it->second.get_mac() << "\"" << endl;
        of_json << "            }" << endl;
        of_json << "          }," << endl;
    }

    of_json << " {}" << endl;
    of_json << " ]" << endl;
    of_json << "}" << endl;

    of_json << endl;
    of_json.close();
}
