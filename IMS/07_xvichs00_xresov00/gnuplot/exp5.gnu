set terminal png size 400, 300 font "Helvetica, 9"

set key samplen 0
set key spacing 1
set key left

set output "grafy/exp5.png"

set xlabel 'T'
set ylabel 'J' offset 1.5,0,0
set xrange [0:150]
set yrange [0.0:0.25]
set ytics 0.05
set xtics 25 

plot \
"vysledky/exp5a.dat" title "NxN = 16, D = 50, SYNCHRONIZED" with points,\
"vysledky/exp5b.dat" title "NxN = 16, D = 50, GREEN_WAVE" with points
