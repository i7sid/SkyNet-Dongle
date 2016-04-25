#
# Makefile
# Michael Zapf, 2016-04-26 15:46
#

.PHONY: all clean pkg.rev

all:
	phing build

clean:
	rm -rf base-web*.zip

schema-create:
	./vendor/bin/doctrine orm:schema-tool:create

schema-update:
	./vendor/bin/doctrine orm:schema-tool:update --force

# vim:ft=make
#
