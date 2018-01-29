do for [i=0:600] {

reset
n=50 #number of intervals
max=10. #max value
min=0. #min value
width=(max-min)/n #interval width

#function used to map a value to the intervals
hist(x,width)=width*floor(x/width)+width/2.0
set xrange [min:max]
set yrange [0:400]

#to put an empty boundary around the
#data inside an autoscaled graph.
set offset graph 0.05,0.05,0.05,0.0
set xtics min,(max-min)/5,max
set boxwidth width*0.9
set tics out nomirror
set style fill solid 0.5 #fillstyle

plot_title = sprintf("t=%4.2f", i/20.," Myr")
set title plot_title 

set ylabel "N"
set xlabel "v [km/s]"

#count and plot

set term png #output terminal and file
set output 'vel_disp_'.i.'.png'

plot 'output_'.i.'.dat' u (hist(sqrt($6*$6+$7*$7+$8*$8),width)):(1.0) smooth freq w boxes lc rgb"red" notitle

}
