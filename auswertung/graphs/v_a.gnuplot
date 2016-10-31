#data_in_raw     = "../data/".station."-wind-".datum."-raw.csv"
#data_in_raw_dir = "../data/".station."-wind-".datum."-dir-raw.csv"
#data_in_model   = "../data/wind-".datum."-modeled.csv"

data_in_model = in_file

#dfrom   = "2016-07-19 09:24:00"
#duntil  = "2016-07-19 15:30:00"

seconds = strptime("%Y-%m-%d-%H-%M:%S", datum)
datum_de = strftime("%d.%m.%Y", seconds)

set datafile separator ","
set datafile missing "NA"
set encoding utf8

stats data_in_model u (strptime("%Y-%m-%d %H:%M:%S",strcol(1))):2 prefix "A" nooutput
if (!exists("dfrom")) { dfrom = A_min_x; }
if (!exists("duntil")) { duntil = A_max_x; }


set title "Aufwind am ".datum_de
#set title "Aufwind am ".datum_de.", Messpunkt ".station

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
set style line 1 lt 1 linecolor rgb "#0000DD"
set style line 2 lt 1 linecolor rgb "#9900CC"
set style line 3 lt 1 linecolor rgb "#0099DD"
set style line 4 lt 1 linecolor rgb "#00DD99"
set style line 5 lt 1 linecolor rgb "black"


#plot data_in_raw  using 1:2 ls 5 title 'Windgeschwindigkeit' , \
#    data_in_raw_dir  using 1:2 axes x1y2 ls 4 title 'Windrichtung'

plot data_in_model using 1:5 axes x1y1 ls 1 title 'Windgeschwindigkeit' , \
    data_in_model using 1:13 axes x1y1 ls 2 title 'Windgeschwindigkeit' #, \
    #data_in_model using 1:26 axes x1y2 ls 3 title 'Windrichtung', \
    #data_in_model using 1:29 axes x1y2 ls 4 title 'Windrichtung'



# vim: set ft=gnuplot:
