#
# Makefile
# Michael Zapf, 2016-04-08 12:52
#

RM = rm -rf
GXX = g++
GREP = grep
GREP_ARGS  = --exclude={Makefile,*.swp,*.swo,*~}
GREP_ARGS += --exclude-dir=".git" -n --color=auto

MAC_ADDR = 03:6B:84:A0:75:CC

CXXFLAGS = -std=c++0x -I"./src" -I"../firmware/src/misc" \
		   -I"../firmware/src/mac" -I"../firmware/src" -O3 -s -Wall \
		   -fmessage-length=0 -Wno-packed-bitfield-compat \
		   -Werror -Wno-unused-result \
		   -DPKT_DBG_OVERHEAD=\(4\) \
		   -DMAC_ADDR=\"$(MAC_ADDR)\"

LDFLAGS = -pthread -lmenu -lform -lncurses


CXXFLAGS += -g -rdynamic

SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

.PHONY: all clean todo

all: rangetest

rangetest: $(SRC)
	$(GXX) -o $@ $(CXXFLAGS) $? $(LDFLAGS)

clean:
	$(RM) rangetest

todo:
	-$(GREP) $(GREP_ARGS) -i "TODO" . -R


# vim:ft=make
#
