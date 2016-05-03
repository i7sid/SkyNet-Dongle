/**
 *  @file
 *
 *  @date 	03.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef DB_DB_H_
#define DB_DB_H_

#include <mysql++/mysql++.h>

class db : mysqlpp::Connection {
public:
	db(std::string host, std::string user, std::string pass, std::string dbname);

	void record_entity(int station, std::string timestamp, std::string value);

private:
	std::string host;
	std::string user;
	std::string pass;
	std::string dbname;

	unsigned long long experiment = 0;

	mysqlpp::Query q_insert_entity;
	mysqlpp::Query q_insert_experiment;
};


#endif /* DB_DB_H_ */
