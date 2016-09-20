data_in         = "../data/".station."-wind-".datum."-diff.csv"
starts_in       = "../data/starts-".datum.".csv"




seconds = strptime("%Y-%m-%d-%H-%M:%S", datum)
datum_de = strftime("%d.%m.%Y", seconds)

set datafile separator ","
set datafile missing "NA"
set encoding utf8

stats data_in u (strptime("%Y-%m-%d %H:%M:%S",strcol(1))):2 prefix "A" nooutput
if (!exists("dfrom")) { dfrom = A_min_x; }
if (!exists("duntil")) { duntil = A_max_x; }


set title "Differenz der gemittelten Windgeschwindigkeit am ".datum_de.", Messpunkt ".station

set timefmt "%Y-%m-%d %H:%M:%S"
set style data lines
set nokey
set xzeroaxis

set xdata time
set format x "%H:%M"
set xlabel "Zeit (UTC)"
set ylabel "Windgeschwindigkeit (km/h)"

set xrange [dfrom:duntil]
set yrange [-15:15]

set style increment user
set style line 1 lt 1 linecolor rgb "green"
set style line 2 lt 1 linecolor rgb "red"
set style line 5 lt 1 linecolor rgb "black"

plot data_in using 1:2 ls 5, \
    starts_in \
    u 2:(A_min_y):(0):(abs(A_min_y) + A_max_y):3 with vectors nohead lc var





# vim: set ft=gnuplot:
