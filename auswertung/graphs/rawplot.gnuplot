data_in_raw     = "../data/".station."-wind-".datum."-raw.csv"
data_in_raw_dir = "../data/".station."-wind-".datum."-dir-raw.csv"
data_in_model   = "../data/wind-".datum."-modeled.csv"

#dfrom   = "2016-07-19 09:24:00"
#duntil  = "2016-07-19 15:30:00"

seconds = strptime("%Y-%m-%d-%H-%M:%S", datum)
datum_de = strftime("%d.%m.%Y", seconds)

set datafile separator ","
set datafile missing "NA"
set encoding utf8

stats data_in_raw u (strptime("%Y-%m-%d %H:%M:%S",strcol(1))):2 prefix "A" nooutput
if (!exists("dfrom")) { dfrom = A_min_x; }
if (!exists("duntil")) { duntil = A_max_x; }


set title "Wind am ".datum_de." (ungefiltert), Messpunkt ".station

set timefmt "%Y-%m-%d %H:%M:%S"
set style data lines
set key top left
set xzeroaxis

set xdata time
set format x "%H:%M"
set xlabel "Zeit (UTC)"
set ylabel "Windgeschwindigkeit (km/h)"
set y2label "Windrichtung (°)"

set xrange [dfrom:duntil]
set yrange [0:35]
set y2range [0:380]
set y2tics
set tics nomirror

set style increment user
set style line 1 lt 1 linecolor rgb "green"
set style line 2 lt 1 linecolor rgb "red"
#set style line 4 lt 1 linecolor rgb "#ccccff"
set style line 4 lt 1 lc rgb "#9999dd" lw 1 pt 7 ps 0.25
set style line 5 lt 1 linecolor rgb "black"


#plot data_in_raw  using 1:2 ls 5 title 'Windgeschwindigkeit' , \
#    data_in_raw_dir  using 1:2 axes x1y2 ls 4 title 'Windrichtung'

plot data_in_raw_dir  using 1:2 axes x1y2 ls 4 title 'Windrichtung' , \
    data_in_raw  using 1:2 ls 5 title 'Windgeschwindigkeit', \
    data_in_model using 1:30 ls 2 title 'Aufwindzentrum gefunden'



# vim: set ft=gnuplot:
