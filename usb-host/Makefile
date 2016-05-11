#
# Makefile
# Michael Zapf, 2016-04-08 12:52
#

RM = rm -rf
GXX = g++

MAC_ADDR = 03:6B:84:A0:75:CB

CXXFLAGS = -std=c++0x -I"./src" -I"../firmware/src/misc" \
		   -I"../firmware/src/mac" -I"../firmware/src" -O3 -s -Wall \
		   -fmessage-length=0 -Wno-packed-bitfield-compat \
		   -Werror -Wno-unused-result \
		   -DPKT_DBG_OVERHEAD=\(4\) \
		   -DMAC_ADDR=\"$(MAC_ADDR)\"

LDFLAGS = -pthread -lmenu -lform -lncurses

ifeq ($(NO_TAP),1)
	CXXFLAGS += -DNO_TAP
else
endif

ifeq ($(NO_DB),1)
	CXXFLAGS += -DNO_DB
else
	CXXFLAGS += -I"/usr/include/mysql"
	LDFLAGS += -lmysqlclient -lmysqlpp
endif

CXXFLAGS += -g -rdynamic

 
SRC = $(wildcard src/*.cpp) $(wildcard src/gui/*.cpp) $(wildcard src/db/*.cpp) ../firmware/src/mac/mac_frame_data.c

.PHONY: all clean

all: skynet-usb-host

skynet-usb-host: $(SRC)
	$(GXX) -o $@ $(CXXFLAGS) $? $(LDFLAGS)

clean:
	$(RM) skynet-usb-host



# vim:ft=make
#
