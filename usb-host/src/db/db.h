/**
 *  @file
 *
 *  @date 	03.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef NO_DB

#ifndef DB_DB_H_
#define DB_DB_H_

#include <mysql++/mysql++.h>

#define DB_TYPE_GPS				(1)
#define DB_TYPE_COMPASS			(2)
#define DB_TYPE_WIND_SPEED		(3)
#define DB_TYPE_WIND_DIR_RAW	(4)
#define DB_TYPE_WIND_DIR_COMP	(5)


class db : mysqlpp::Connection {
public:
	db(std::string host, std::string user, std::string pass, std::string dbname);

	void record_entity(int station, int type, std::string timestamp, std::string value);

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
#endif // NO_DB
