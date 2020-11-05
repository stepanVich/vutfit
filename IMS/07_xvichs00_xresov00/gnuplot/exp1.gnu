set terminal png size 400, 300 font "Helvetica, 9"

set key samplen 0
set key spacing 1
set key left

set output "grafy/exp1.png"

set xlabel 'T'
set ylabel 'J' offset 1.5,0,0
set xrange [0:150]
set ytics 0.05
set xtics 25 

plot \
"vysledky/exp1a.dat" title "rho = 0.05, R = 0.1" with points,\
"vysledky/exp1b.dat" title "rho = 0.20, R = 0.1" with points,\
"vysledky/exp1c.dat" title "rho = 0.50, R = 0.1" with points,\
"vysledky/exp1d.dat" title "rho = 0.70, R = 0.1" with points
