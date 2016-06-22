/**
 * @file
 *
 * @brief   TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#ifndef CSV_H
#define CSV_H

#include <string>
#include <fstream>
#include "output.h"

class CsvOutput : public DataOutput {
    public:
        CsvOutput();
        CsvOutput(std::string filename);
        ~CsvOutput() {};

        void set_filename(std::string filename);
        void updated(void);

    private:
        std::string filename;

};


#endif /* !CSV_H */
