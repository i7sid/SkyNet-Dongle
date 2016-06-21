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

class JsonOutput : public DataOutput {
    public:
        JsonOutput(std::string filename);
        ~JsonOutput() {};
        void updated(void);

    private:
        std::string filename;

};


#endif /* !JSON_H */
