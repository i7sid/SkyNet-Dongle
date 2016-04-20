#
# Makefile
# Michael Zapf, 2016-02-04 19:43
#

all: mac-test

clean:
	rm -rf mac-test

mac-test:
	gcc -Werror -std=c99 -I../../firmware/src/misc/ -o mac-test $(wildcard ../../firmware/src/mac/*.c) mac-test.c

# vim:ft=make
#
