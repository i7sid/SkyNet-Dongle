#
# Makefile
# Michael Zapf, 2016-09-19 11:00
#


# configuration for current build
DATE	=2016-09-09
TIME	=11-07-32




# configuration for environment; NORMALLY you do not want to change these
R		= Rscript
RFLAGS	= --vanilla
RWIND	= ./math/wind_analyze.r

RM		= rm -rf
ECHO	= /bin/echo -ne

GNUPLOT				= gnuplot
GNUPLOT_TERM_PNG	= set terminal pngcairo size 1450,500 enhanced font 'DejaVu-Sans,10'
GNUPLOT_TERM_PDF	= set terminal pdf size 29cm,21cm enhanced font 'DejaVu-Sans,10'
GNUPLOT_BASEDIR		= ./graphs
GNUPLOT_FULL		= fullplot_draw.gnuplot
GNUPLOT_DIFF		= flugplot_draw.gnuplot
GNUPLOT_FULL_DIR	= fullplot_dir_draw.gnuplot
GNUPLOT_DIFF_DIR	= flugplot_dir_draw.gnuplot
GNUPLOT_SCORE		= flugplot_draw_score.gnuplot

PHP			= php
SEPARATE	= $(PHP) ./math/separate.php
MODEL		= $(PHP) ./math/model.php
WIND_VIS	= $(PHP) ./math/wind2geojson.php
POS_MEAN	= $(PHP) ./math/csv-pos-mean.php
MODEL_CALL	= $(PHP) ./math/model_call.php

BEFORE_TITLE	= \\n
AFTER_TITLE  	= \\n
NO_COLOR		= \x1b[0m
GREEN			= \x1b[32;01m
RED				= \x1b[31;01m
YELLOW			= \x1b[33;01m


# automatic stuff
DATETIME	= $(DATE)-$(TIME)

TTY_CAN_COL := $(shell [ -t 0 ] && test $$(tput colors) -ge 8 && echo 1 )

ifeq ($(TTY_CAN_COL),1)
BEFORE_TITLE := "$(YELLOW)$(BEFORE_TITLE)"
AFTER_TITLE := "$(AFTER_TITLE)$(NO_COLOR)"
else
BEFORE_TITLE := "$(BEFORE_TITLE)\#\#\#\#\#   "
AFTER_TITLE := "     \#\#\#\#\#$(AFTER_TITLE)"
endif



WIND_SRC 	= ./data/raw/wind-data-$(DATETIME).csv

POS_SRC  	= $(patsubst ./data/raw/wind-%.csv,./data/raw/pos-%.csv,$(WIND_SRC))
STARTS_SRC	= $(patsubst ./data/raw/wind-%.csv,./data/raw/starts-%.csv,$(WIND_SRC))
WIND_RAW	= $(patsubst ./data/raw/%.csv,./data/%-raw.csv,$(WIND_SRC))


.PHONY: all separate process graphs model wind-vis clean

all: separate process model graphs wind-vis


separate: $(WIND_SRC) $(POS_SRC)
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Separating wind and position data files..."
	@$(ECHO) $(AFTER_TITLE)
	$(SEPARATE) $(WIND_SRC) ./data/MAC-wind-$(DATETIME).csv
	$(SEPARATE) $(POS_SRC) ./data/MAC-pos-$(DATETIME).csv


process-recursive-wind: $(wildcard ./data/*-wind-$(DATETIME).csv)
	make $(patsubst %.csv,%-raw.csv,$(wildcard ./data/*-wind-$(DATETIME).csv))

process-recursive-pos: $(wildcard ./data/*-pos-$(DATETIME).csv)
	make $(patsubst %.csv,%-mean.csv,$(wildcard ./data/*-pos-$(DATETIME).csv))

process: separate
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Process wind data..."
	@$(ECHO) $(AFTER_TITLE)
	make process-recursive-wind
	make process-recursive-pos

model: process
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Calculating wind model..."
	@$(ECHO) $(AFTER_TITLE)
	$(MODEL_CALL) ./data > ./data/wind-$(DATETIME)-modeled.csv

geojson: model
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Generating GeoJSON file..."
	@$(ECHO) $(AFTER_TITLE)
	$(WIND_VIS) ./data/wind-$(DATETIME)-modeled.csv  ./data/*-pos-$(DATETIME)-mean.csv > ./data/wind-$(DATETIME)-vis.json



./graphs/%-wind-$(DATETIME)-full.png: ./data/%-wind-$(DATETIME)-raw.csv ./data/%-wind-$(DATETIME)-5min.csv ./data/%-wind-$(DATETIME)-15min.csv ./data/%-wind-$(DATETIME)-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_FULL) > $(@F)

./graphs/%-wind-$(DATETIME)-diff.png: ./data/%-wind-$(DATETIME)-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-full.png: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/%-wind-$(DATETIME)-dir-5min.csv ./data/%-wind-$(DATETIME)-dir-15min.csv ./data/%-wind-$(DATETIME)-dir-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_FULL_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-diff.png: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_DIFF_DIR) > $(@F)


./graphs/$(DATETIME)-score.png: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_PNG); " ./graphs/$(GNUPLOT_SCORE) \
		> ./graphs/$(DATETIME)-score.png


graphs-recursive: $(wildcard ./data/*-wind-$(DATETIME)-raw.csv)
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-full.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-full.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make ./graphs/$(DATETIME)-score.png


graphs: separate process model ./data/starts-$(DATETIME).csv
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Generating graphs..."
	@$(ECHO) $(AFTER_TITLE)
	make graphs-recursive




./data/%-wind-$(DATETIME)-raw.csv: ./data/%-wind-$(DATETIME).csv
	$(R) $(RFLAGS) $(RWIND) $? $(subst -raw.csv,,$@)

./data/%-wind-$(DATETIME)-5min.csv: ./data/raw/%-wind-$(DATETIME).csv
	$(R) $(RFLAGS) $(RWIND) $? $(subst -5min.csv,,$@)

./data/%-wind-$(DATETIME)-15min.csv: ./data/raw/%-wind-$(DATETIME).csv
	$(R) $(RFLAGS) $(RWIND) $? $(subst -15min.csv,,$@)

./data/%-wind-$(DATETIME)-diff.csv: ./data/raw/%-wind-$(DATETIME).csv
	$(R) $(RFLAGS) $(RWIND) $? $(subst -diff.csv,,$@)

./data/%-pos-$(DATETIME)-mean.csv: ./data/%-pos-$(DATETIME).csv
	$(POS_MEAN) $? > $@





./data/starts-%.csv: ./data/raw/starts-%.csv
	cp $? $@


clean:
	@$(ECHO) $(BEFORE_TITLE)
	@$(ECHO) "Cleaning up..."
	@$(ECHO) $(AFTER_TITLE)
	$(RM) ./data/*.json
	$(RM) ./data/*.csv
	$(RM) ./graphs/*.png
	$(RM) ./graphs/*.pdf





# vim:ft=make
#
