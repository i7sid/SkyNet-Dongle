data_in_raw     = "../data/".station."-wind-".datum."-raw.csv"
data_in_5min    = "../data/".station."-wind-".datum."-5min.csv"
data_in_15min   = "../data/".station."-wind-".datum."-15min.csv"
data_in_diff    = "../data/".station."-wind-".datum."-diff.csv"
starts_in       = "../data/starts-".datum.".csv"

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

set style increment user
set style line 1 lt 1 linecolor rgb "green"
set style line 2 lt 1 linecolor rgb "red"
set style line 5 lt 1 linecolor rgb "black"

plot data_in_raw  using 1:2 ls 5 title 'Windgeschwindigkeit', \
    data_in_5min  using 1:2 ls 2 lw 2 title 'gemittelt über 5min', \
    data_in_15min using 1:2 ls 1 lw 2 title 'gemittelt über 15min', \
    data_in_diff  using 1:2 ls 4 lw 2 title 'Differenz', \
    starts_in u 2:(A_min_y-10):(0):(abs(A_min_y) + A_max_y + 10):3 \
    with vectors nohead lc var notitle





# vim: set ft=gnuplot:
