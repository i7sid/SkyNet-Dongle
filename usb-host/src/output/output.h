/**
 * @file
 *
 * @brief   Interface for automatic export of data.
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <forward_list>
#include "station.h"

class DataOutput {
    public:
        ~DataOutput() {};
//        virtual ~DataOutput();
        virtual void updated(station *s) = 0;

        static void update_all(station *s);
        static void register_output(DataOutput *o);

    protected:
        static std::forward_list<DataOutput*> output_handlers;

};


#endif /* !OUTPUT_H */
