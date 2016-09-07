/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-06-22
 */

#include <cmath>
#include <iostream>
#include <iomanip>
#include "station.h"
#include "fifo.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;



FifoOutput::FifoOutput() : filename("") {}

FifoOutput::~FifoOutput() {
    close(fd);                  // close file descriptor
    unlink(filename.c_str());   // delete file
};


bool FifoOutput::create(std::string filename) {
    // create FIFO

    this->filename = filename;

    if((mkfifo(filename.c_str(), O_RDWR)) == -1) {
        return false;
    }
    return true;
}

bool FifoOutput::open_fifo(std::string filename) {
    if (fd) return true;

    this->filename = filename;

    // make it world readable and writeable
    chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP |
                    S_IROTH | S_IWOTH | S_IXOTH);

    // open FIFO
    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        //perror("FIFO open");
        return false;
    }

    return true;
}

void FifoOutput::updated(station *s) {
    char buf[] = "Updraft\n0000\n00000000000000000000";

    bool opened = open_fifo(filename.c_str());
    if (!opened) return;

    // TODO get updraft data and write it to buffer

    // write buffer + terminating null byte
    write(fd, buf, strlen(buf) + 1);
}
