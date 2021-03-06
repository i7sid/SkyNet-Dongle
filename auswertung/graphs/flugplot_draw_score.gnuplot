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
set key left
set xzeroaxis
set samples 10000

set xdata time
set format x "%H:%M"
set xlabel "Zeit (UTC)"
set ylabel "Aufwind-Score"

set xrange [dfrom:duntil]
set yrange [0:100]
#set yrange [0:50]

set style increment user
set style line 1 lt 1 linecolor rgb "green"
set style line 2 lt 1 linecolor rgb "red"
set style line 3 lt 1 linecolor rgb "#0000FF"
set style line 5 lt 1 linecolor rgb "black"

plot data_in using 1:23 ls 5 title "Score" , \
     data_in using 1:31 ls 2  title "Nähe des Aufwindzentrums" #, \

    # data_in using 1:30 ls 3 title "Punkt berechnet" , \
    #starts_in \
    #u 2:(A_min_y):(0):(abs(A_min_y) + A_max_y):3 with vectors nohead lc var




# vim: set ft=gnuplot:
