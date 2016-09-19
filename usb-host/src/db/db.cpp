/*
 *  @file
 *
 *  @date 	03.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef NO_DB

#include "db.h"


using namespace std;
using namespace mysqlpp;

db::db(std::string host, std::string user, std::string pass, std::string dbname) :
		Connection(true), host(host), user(user), pass(pass), dbname(dbname),
		q_insert_entity(this, true, string("").c_str()),
		q_insert_station(this, true, string("").c_str()),
		q_get_station(this, true, string("").c_str()),
		q_insert_experiment(this, true, string("").c_str()) {

	this->connect(dbname.c_str(), host.c_str(), user.c_str(), pass.c_str(), 0);

	q_insert_entity << "INSERT INTO entities ( `station_id` , `experiment_id` , `timestamp` , `value` , `entity_type_id` ) " <<
			" VALUES ( '%0:station_id' , '%1:experiment_id' ,  '%2:timestamp' ,  '%3:value', '%4:type' );";
	q_insert_entity.parse();


	q_insert_station << "INSERT INTO stations ( `longitude` , `latitude` , `mac` ) " <<
				" VALUES ( '%0:longitude' , '%1:latitude' ,  '%2:mac' );";
	q_insert_station.parse();


	q_get_station << "SELECT * FROM stations WHERE `mac`='%0:mac';";
	q_get_station.parse();



	q_insert_experiment << "INSERT INTO experiments () VALUES ();";
	SimpleResult r_exp = q_insert_experiment.execute();
	experiment = r_exp.insert_id();


}

int db::get_station(std::string mac) {
	StoreQueryResult res = q_get_station.store(mac.c_str(), "");

	// found station
	if (res.num_rows() > 0) {
		mysqlpp::StoreQueryResult::const_iterator it;
		mysqlpp::Row row = *(res.begin());
		return row[0];
	}

	cerr << mac << endl;
	// new station to db
	SimpleResult r_exp = q_insert_station.execute("0", "0", mac.c_str());
	return r_exp.insert_id();
}


void db::record_entity(int station, int type, std::string timestamp, std::string value) {
    try {
	/*StoreQueryResult res =*/ q_insert_entity.store(std::to_string(station).c_str(),
									std::to_string(experiment).c_str(), timestamp.c_str(),
									value.c_str(), std::to_string(type).c_str());

    }
    catch (std::exception ex) {
        cerr << endl << "MySQL exception occurred." << endl;
        cerr << "Station:   " << station << endl;
        cerr << "Timestamp: " << timestamp << endl;
        cerr << "value:   " << value << endl << endl;
       
        cerr << "Query: " << q_insert_entity.str(
                std::to_string(station).c_str(),
									std::to_string(experiment).c_str(), timestamp.c_str(),
									value.c_str(), std::to_string(type).c_str()
                ) << endl;
    }
	///*SimpleResult result = */ q_insert_entity.execute();
}



#endif // NO_DB
