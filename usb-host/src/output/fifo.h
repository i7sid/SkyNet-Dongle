/**
 * @file
 *
 * @brief   TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#ifndef FIFO_H
#define FIFO_H

#include <string>
#include <fstream>
#include "output.h"

class FifoOutput : public DataOutput {
    public:
        FifoOutput();
        FifoOutput(std::string filename);
        ~FifoOutput();

        bool create(std::string filename);
        bool open_fifo(std::string filename);

        void updated(station *s);

    private:
        std::string     filename;
        int             fd = 0;
//        std::string last_wind_time = "";
//        std::string last_pos_time = "";

};


#endif /* !FIFO_H */
