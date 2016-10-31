#data_in = "../data/output-".datum."-station".station."-diff.csv"

data_in = in_file
starts_in = in_starts

seconds = strptime("%Y-%m-%d-%H-%M-%S", datum)
datum_de = strftime("%d.%m.%Y", seconds)

set datafile separator ","
set datafile missing "NA"
set encoding utf8

stats data_in u (strptime("%Y-%m-%d %H:%M:%S",strcol(1))):23 prefix "A" nooutput

if (!exists("dfrom")) { dfrom = A_min_x; }
if (!exists("duntil")) { duntil = A_max_x; }


set title "Windscore am ".datum_de

set timefmt "%Y-%m-%d %H:%M:%S"
set style data lines
set nokey
set xzeroaxis

set xdata time
set format x "%H:%M"
set xlabel "Zeit (UTC)"
set ylabel "Aufwind-Score"

set xrange [dfrom:duntil]
set yrange [0:60]
set y2range [0:30]
set tics nomirror
set y2tics
set y2label "Aufwindgeschwindigkeit (m/s)"

set style increment user
set style line 1 lt 1 linecolor rgb "green"
set style line 2 lt 1 linecolor rgb "red"
set style line 3 lt 1 linecolor rgb "#0000FF"
set style line 5 lt 1 linecolor rgb "black"

#plot data_in using 1:32 ls 5, \
#    data_in using 1:33 ls 3, \
#    starts_in \
#    u 2:(A_min_y):(0):(abs(A_min_y) + A_max_y):3 with vectors nohead lc var


set key

plot data_in using 1:31 axes x1y2 ls 5 title "Station 5d5d (Aufwindgeschwindigkeit)", \
    data_in using 1:32 ls 3  title "Station 5d5d (Score-Berechnung)"#, \




# vim: set ft=gnuplot:
