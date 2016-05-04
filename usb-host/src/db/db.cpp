/*
 *  @file
 *
 *  @date 	03.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "db.h"


using namespace std;
using namespace mysqlpp;

db::db(std::string host, std::string user, std::string pass, std::string dbname) :
		Connection(true), host(host), user(user), pass(pass), dbname(dbname),
		q_insert_entity(this, true, string("").c_str()),
		q_insert_experiment(this, true, string("").c_str()) {

	this->connect(dbname.c_str(), host.c_str(), user.c_str(), pass.c_str(), 0);

	q_insert_entity << "INSERT INTO entities ( `station_id` , `experiment_id` , `timestamp` , `value` , `entity_type_id` ) " <<
			" VALUES ( '%0:station_id' , '%1:experiment_id' ,  '%2:timestamp' ,  '%3:value', '%4:type' );";
	q_insert_entity.parse();


	q_insert_experiment << "INSERT INTO experiments () VALUES ();";
	SimpleResult r_exp = q_insert_experiment.execute();
	experiment = r_exp.insert_id();
}


void db::record_entity(int station, int type, std::string timestamp, std::string value) {
	/*StoreQueryResult res =*/ q_insert_entity.store(std::to_string(station).c_str(),
									std::to_string(experiment).c_str(), timestamp.c_str(),
									value.c_str(), std::to_string(type).c_str());

	///*SimpleResult result = */ q_insert_entity.execute();
}



