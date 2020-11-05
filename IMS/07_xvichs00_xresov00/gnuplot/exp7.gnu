set terminal png size 400, 300 font "Helvetica, 9"

set key samplen 0
set key spacing 1
set key left

set output "grafy/exp7.png"

set xlabel 'T'
set ylabel 'J' offset 1.5,0,0
set xrange [0:150]
set ytics 0.05
set xtics 25 

plot \
"vysledky/exp7a.dat" title "N = 10, rho = 0.7, SYNCHRONIZED" with points,\
"vysledky/exp7b.dat" title "N = 10, rho = 0.7, RANDOMIZED" with points
