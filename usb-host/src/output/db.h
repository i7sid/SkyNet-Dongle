/**
 * @file
 *
 * @brief   Database logging.
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#ifndef NO_DB
#ifndef OUTPUTDB_H
#define OUTPUTDB_H

#include <string>
#include <fstream>
#include "output.h"
#include "db/db.h"

class DbOutput : public DataOutput {
    public:
        DbOutput();
        ~DbOutput() {};

        void updated(station *s);

    private:
        db dbcon;
        std::string last_wind_time = "";
        std::string last_pos_time = "";

};



#endif /* !OUTPUTDB_H */
#endif
