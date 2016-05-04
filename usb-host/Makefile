#
# Makefile
# Michael Zapf, 2016-04-08 12:52
#

#TODO command line flag for NO_MYSQL and NO_TAP

RM = rm -rf
GXX = g++
CXXFLAGS = -std=c++0x -I"./src" -I"../firmware/src/misc" \
		   -I"../firmware/src/mac" -I"../firmware/src" -O3 -s -Wall \
		   -fmessage-length=0 -Wno-packed-bitfield-compat \
		   -Werror -Wno-unused-result \
		   -DPKT_DBG_OVERHEAD=\(8\) \

#CXXFLAGS += -DNO_TAP
#CXXFLAGS += -DNO_DB
CXXFLAGS += -I"/usr/include/mysql"
CXXFLAGS += -g -rdynamic

LDFLAGS = -pthread -lmenu -lform -lncurses
LDFLAGS += -lmysqlclient -lmysqlpp
 
SRC = $(wildcard src/*.cpp) $(wildcard src/gui/*.cpp) $(wildcard src/db/*.cpp) ../firmware/src/mac/mac_frame_data.c

.PHONY: all clean

all: skynet-usb-host

skynet-usb-host: $(SRC)
	$(GXX) -o $@ $(CXXFLAGS) $? $(LDFLAGS)

clean:
	$(RM) skynet-usb-host



# vim:ft=make
#
