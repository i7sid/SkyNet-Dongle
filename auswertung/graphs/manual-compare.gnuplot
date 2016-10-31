station         = "5d5d"
datum           = "2016-07-08-09-12-35"

data_in_raw     = "../data/".station."-wind-".datum."-raw.csv"
data_in_1min    = "../data/".station."-wind-".datum."-1min.csv"
data_in_5min    = "../data/".station."-wind-".datum."-5min.csv"
data_in_15min   = "../data/".station."-wind-".datum."-15min.csv"
data_in_30min   = "../data/".station."-wind-".datum."-30min.csv"
data_in_diff    = "../data/".station."-wind-".datum."-diff.csv"
data_in_vect    = "../data/".station."-wind-".datum."-vector.csv"
data_in_vect_1  = "../data/".station."-wind-".datum."-vector-1.csv"
data_in_vect_15 = "../data/".station."-wind-".datum."-vector-15.csv"
data_in_vect_raw= "../data/".station."-wind-".datum."-vector-raw.csv"

seconds = strptime("%Y-%m-%d-%H-%M:%S", datum)
datum_de = strftime("%d.%m.%Y", seconds)

set datafile separator ","
set datafile missing "NA"
set encoding utf8

stats data_in_raw u (strptime("%Y-%m-%d %H:%M:%S",strcol(1))):2 prefix "A" nooutput
if (!exists("dfrom")) { dfrom = A_min_x; }
if (!exists("duntil")) { duntil = A_max_x; }


set title "Wind am ".datum_de.", Messpunkt ".station

set timefmt "%Y-%m-%d %H:%M:%S"
set style data lines
set key top left
set xzeroaxis

set xdata time
set format x "%H:%M"
set xlabel "Zeit (UTC)"
set ylabel "Windgeschwindigkeit (km/h)"

set xrange [dfrom:duntil]
set yrange [-10:35]
set y2tics

set style increment user
set style line 1 lt 1 linecolor rgb "#999999"
set style line 2 lt 1 linecolor rgb "#4444FF"
set style line 3 lt 1 linecolor rgb "#44FF44"
set style line 4 lt 1 linecolor rgb "#FF4444"
set style line 5 lt 1 linecolor rgb "#FF44FF"

set terminal svg size 1450,500 font 'DejaVu-Sans,10'

plot data_in_vect_raw using 1:3  ls 1 title 'raw', \
    data_in_vect_1 using 1:3  ls 2 title '1min', \
    data_in_vect using 1:3  ls 3 title '5min', \
    data_in_vect_15 using 1:3  ls 4 title '15min'






# vim: set ft=gnuplot:
