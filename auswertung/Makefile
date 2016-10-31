#
# Makefile
# Michael Zapf, 2016-09-19 11:00
#


# configuration for current build
#DATE	=2016-09-09
#TIME	=11-07-32
#DATE	=2016-07-19
#TIME	=09-33-49
DATE	=2016-07-08
TIME	=09-12-35




# configuration for environment; NORMALLY you do not want to change these
R		= Rscript
RFLAGS	= --vanilla
RWIND	= ./math/wind_analyze.r

RM		= rm -rf
ECHO	= /bin/echo -ne

GNUPLOT				= gnuplot
GNUPLOT_TERM_PNG	= set terminal pngcairo size 1450,500 enhanced font 'DejaVu-Sans,10'
GNUPLOT_TERM_PDF	= set terminal pdf size 19cm,6cm enhanced font 'DejaVu-Sans,10'
#GNUPLOT_TERM_PDF	= set terminal pdf size 29cm,10cm enhanced font 'DejaVu-Sans,10'
GNUPLOT_TERM_SVG	= set terminal svg size 1450,500 font 'DejaVu-Sans,10'
GNUPLOT_BASEDIR		= ./graphs
GNUPLOT_FULL		= fullplot_draw.gnuplot
GNUPLOT_DIFF		= flugplot_draw.gnuplot
GNUPLOT_FULL_DIR	= fullplot_dir_draw.gnuplot
GNUPLOT_DIFF_DIR	= flugplot_dir_draw.gnuplot
GNUPLOT_COMPARE		= compare.gnuplot
GNUPLOT_COMPARE_DIR	= compare-dir.gnuplot
GNUPLOT_COMPARE_DIFF		= compare-diff.gnuplot
GNUPLOT_COMPARE_DIR_DIFF	= compare-dir-diff.gnuplot
GNUPLOT_SCORE		= flugplot_draw_score.gnuplot
#GNUPLOT_SCORES		= flugplot_draw_station_scores.gnuplot
GNUPLOT_V_A			= v_a.gnuplot
GNUPLOT_RAW			= rawplot.gnuplot

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


.PHONY: all separate process graphs model clean geojson

all: separate process model geojson graphs


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

./graphs/%-wind-$(DATETIME)-raw.png: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_RAW) > $(@F)

./graphs/%-wind-$(DATETIME)-compare.png: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_COMPARE) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-compare.png: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_COMPARE_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-diff.png: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_COMPARE_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-dir-diff.png: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PNG); " $(GNUPLOT_COMPARE_DIR_DIFF) > $(@F)


./graphs/$(DATETIME)-score.png: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_PNG); " ./graphs/$(GNUPLOT_SCORE) \
		> ./graphs/$(DATETIME)-score.png

#./graphs/$(DATETIME)-station-scores.png: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
#	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
#		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
#		$(GNUPLOT_TERM_PNG); " ./graphs/$(GNUPLOT_SCORES) \
#		> ./graphs/$(DATETIME)-station-scores.png

./graphs/$(DATETIME)-v-a.png: ./data/wind-$(DATETIME)-modeled.csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_PNG); " ./graphs/$(GNUPLOT_V_A) \
		> ./graphs/$(DATETIME)-v-a.png


./graphs/%-wind-$(DATETIME)-full.pdf: ./data/%-wind-$(DATETIME)-raw.csv ./data/%-wind-$(DATETIME)-5min.csv ./data/%-wind-$(DATETIME)-15min.csv ./data/%-wind-$(DATETIME)-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_FULL) > $(@F)

./graphs/%-wind-$(DATETIME)-diff.pdf: ./data/%-wind-$(DATETIME)-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-full.pdf: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/%-wind-$(DATETIME)-dir-5min.csv ./data/%-wind-$(DATETIME)-dir-15min.csv ./data/%-wind-$(DATETIME)-dir-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_FULL_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-diff.pdf: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_DIFF_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-raw.pdf: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_RAW) > $(@F)

./graphs/%-wind-$(DATETIME)-compare.pdf: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_COMPARE) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-compare.pdf: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_COMPARE_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-diff.pdf: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_COMPARE_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-dir-diff.pdf: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_PDF); " $(GNUPLOT_COMPARE_DIR_DIFF) > $(@F)


./graphs/$(DATETIME)-score.pdf: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_PDF); " ./graphs/$(GNUPLOT_SCORE) \
		> ./graphs/$(DATETIME)-score.pdf

#./graphs/$(DATETIME)-station-scores.pdf: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
#	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
#		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
#		$(GNUPLOT_TERM_PDF); " ./graphs/$(GNUPLOT_SCORES) \
#		> ./graphs/$(DATETIME)-station-scores.pdf

./graphs/$(DATETIME)-v-a.pdf: ./data/wind-$(DATETIME)-modeled.csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_PDF); " ./graphs/$(GNUPLOT_V_A) \
		> ./graphs/$(DATETIME)-v-a.pdf


./graphs/%-wind-$(DATETIME)-full.svg: ./data/%-wind-$(DATETIME)-raw.csv ./data/%-wind-$(DATETIME)-5min.csv ./data/%-wind-$(DATETIME)-15min.csv ./data/%-wind-$(DATETIME)-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_FULL) > $(@F)

./graphs/%-wind-$(DATETIME)-diff.svg: ./data/%-wind-$(DATETIME)-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-full.svg: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/%-wind-$(DATETIME)-dir-5min.csv ./data/%-wind-$(DATETIME)-dir-15min.csv ./data/%-wind-$(DATETIME)-dir-diff.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_FULL_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-diff.svg: ./data/%-wind-$(DATETIME)-dir-raw.csv ./data/starts-$(DATETIME).csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_DIFF_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-raw.svg: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_RAW) > $(@F)

./graphs/%-wind-$(DATETIME)-compare.svg: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_COMPARE) > $(@F)

./graphs/%-wind-$(DATETIME)-dir-compare.svg: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_COMPARE_DIR) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-diff.svg: ./data/%-wind-$(DATETIME)-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_COMPARE_DIFF) > $(@F)

./graphs/%-wind-$(DATETIME)-compare-dir-diff.svg: ./data/%-wind-$(DATETIME)-dir-raw.csv
	cd $(GNUPLOT_BASEDIR) && $(GNUPLOT) -e "datum='$(DATETIME)'; station='$*'; $(GNUPLOT_TERM_SVG); " $(GNUPLOT_COMPARE_DIR_DIFF) > $(@F)


./graphs/$(DATETIME)-score.svg: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_SVG); " ./graphs/$(GNUPLOT_SCORE) \
		> ./graphs/$(DATETIME)-score.svg

#./graphs/$(DATETIME)-station-scores.svg: ./data/wind-$(DATETIME)-modeled.csv ./data/starts-$(DATETIME).csv
#	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
#		in_starts='./data/starts-$(DATETIME).csv'; datum='$(DATETIME)'; \
#		$(GNUPLOT_TERM_SVG); " ./graphs/$(GNUPLOT_SCORES) \
#		> ./graphs/$(DATETIME)-station-scores.svg

./graphs/$(DATETIME)-v-a.svg: ./data/wind-$(DATETIME)-modeled.csv
	$(GNUPLOT) -e "in_file='./data/wind-$(DATETIME)-modeled.csv'; \
		datum='$(DATETIME)'; \
		$(GNUPLOT_TERM_SVG); " ./graphs/$(GNUPLOT_V_A) \
		> ./graphs/$(DATETIME)-v-a.svg




graphs-recursive: $(wildcard ./data/*-wind-$(DATETIME)-raw.csv)
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-full.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-full.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-raw.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-compare.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-compare-dir-diff.png,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make ./graphs/$(DATETIME)-score.png
	#make ./graphs/$(DATETIME)-station-scores.png
	make ./graphs/$(DATETIME)-v-a.png
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-full.pdf,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-diff.pdf,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-full.pdf,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-diff.pdf,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-raw.pdf,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare.pdf,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-compare.pdf,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare-diff.pdf,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-compare-dir-diff.pdf,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make ./graphs/$(DATETIME)-score.pdf
	#make ./graphs/$(DATETIME)-station-scores.pdf
	make ./graphs/$(DATETIME)-v-a.pdf
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-full.svg,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-diff.svg,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-full.svg,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-diff.svg,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-raw.svg,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare.svg,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-dir-compare.svg,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-raw.csv,%-compare-diff.svg,$(wildcard ./data/*-wind-$(DATETIME)-raw.csv)))
	make $(patsubst ./data/%,./graphs/%,$(patsubst %-dir-raw.csv,%-compare-dir-diff.svg,$(wildcard ./data/*-wind-$(DATETIME)-dir-raw.csv)))
	make ./graphs/$(DATETIME)-score.svg
	#make ./graphs/$(DATETIME)-station-scores.svg
	make ./graphs/$(DATETIME)-v-a.svg


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
	$(RM) ./graphs/*.svg
	$(RM) ./graphs/*.pdf





# vim:ft=make
#
