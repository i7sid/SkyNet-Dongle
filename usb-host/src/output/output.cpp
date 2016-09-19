/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-21
 */

#include "output.h"

using namespace std;

std::forward_list<DataOutput*> DataOutput::output_handlers;

void DataOutput::register_output(DataOutput *o) {
    DataOutput::output_handlers.push_front(o);
}
void DataOutput::update_all(station *s) {
    for (auto it = DataOutput::output_handlers.begin(); it != DataOutput::output_handlers.end(); ++it) {
        (*it)->updated(s);
    }
}
