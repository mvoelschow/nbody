for i in $(seq 0 200); do gnuplot -e "i=$i" plot.gp; done
