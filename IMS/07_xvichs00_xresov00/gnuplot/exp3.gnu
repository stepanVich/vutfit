set terminal png size 400, 300 font "Helvetica, 9"

set key samplen 0
set key spacing 1
set key left

set output "grafy/exp3.png"

set xlabel 'T'
set ylabel 'J' offset 1.5,0,0
set xrange [0:150]
set ytics 0.05
set xtics 25 

plot \
"vysledky/exp3a.dat" title "NxN = 1" with points,\
"vysledky/exp3b.dat" title "NxN = 25" with points
