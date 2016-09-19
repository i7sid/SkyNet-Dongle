/**
 * @file
 *
 * @brief   TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#ifndef JSON_H
#define JSON_H

#include <string>
#include <fstream>
#include "output.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

class JsonOutput : public DataOutput {
    public:
        JsonOutput(std::string filename);
        ~JsonOutput() {};
        void updated(station*);

    private:
        std::string filename;

};


#endif /* !JSON_H */
