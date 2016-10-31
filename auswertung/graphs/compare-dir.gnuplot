data_in_raw     = "../data/".station."-wind-".datum."-dir-raw.csv"
data_in_1min    = "../data/".station."-wind-".datum."-dir-1min.csv"
data_in_5min    = "../data/".station."-wind-".datum."-dir-5min.csv"
data_in_15min   = "../data/".station."-wind-".datum."-dir-15min.csv"
data_in_30min   = "../data/".station."-wind-".datum."-dir-30min.csv"
data_in_diff    = "../data/".station."-wind-".datum."-dir-diff.csv"
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
set ylabel "Windrichtung (°)"

set xrange [dfrom:duntil]
set yrange [-10:390]

set tics nomirror
set y2tics ("N" 0, "O" 90, "S" 180, "W" 270, "N" 360)

set style increment user
set style line 1 lt 1 linecolor rgb "#999999"
set style line 2 lt 1 linecolor rgb "#4444FF"
set style line 3 lt 1 linecolor rgb "#44FF44"
set style line 4 lt 1 linecolor rgb "#FF4444"
set style line 5 lt 1 linecolor rgb "#FF44FF"

plot data_in_raw  using 1:2 ls 1 title 'Windrichtung', \
    data_in_1min  using 1:2 ls 2 lw 2 title 'gemittelt über 1min', \
    data_in_5min  using 1:2 ls 3 lw 2 title 'gemittelt über 5min', \
    data_in_15min using 1:2 ls 4 lw 2 title 'gemittelt über 15min', \
    data_in_30min using 1:2 ls 5 lw 2 title 'gemittelt über 30min'





# vim: set ft=gnuplot:
