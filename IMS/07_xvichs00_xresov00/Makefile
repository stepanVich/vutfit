all: main.cpp cell.cpp
	g++ main.cpp -std=c++11 -o sim

rm:
	rm -rf sim

run: custom 

custom:
	./sim -n 2 -d 10 -i 1000 -V

exp1a:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.05, randomization p = 0.1"
	./sim 1a > vysledky/exp1a.dat

exp1b:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.20, randomization p = 0.1"
	./sim 1b > vysledky/exp1b.dat

exp1c:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerne rychlosti automobilu v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.50, randomization p = 0.1"
	./sim 1c > vysledky/exp1c.dat

exp1d:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.70, randomization p = 0.1"
	./sim 1d > vysledky/exp1d.dat

plot1:
	@echo "Tvorba grafu pro zobrazeni dat z prvnich ctyr simulaci"
	gnuplot	gnuplot/exp1.gnu

exp2a:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.05, randomization p = 0.5"
	./sim 2a > vysledky/exp2a.dat


exp2b:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.20, randomization p = 0.5"
	./sim 2b > vysledky/exp2b.dat

exp2c:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v modelu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.50, randomization p = 0.5"
	./sim 2c > vysledky/exp2c.dat


exp2d:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v systemu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.70, randomization p = 0.5"
	./sim 2d > vysledky/exp2d.dat
plot2:
	@echo "Tvorba grafu pro zobrazeni dat ze 4 az 8 predpripravene exp"
	gnuplot	gnuplot/exp2.gnu
exp3a:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 1min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toku v systemu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 1, D = 100, rho = 0.05, randomization p = 0.1"
	./sim 3a > vysledky/exp3a.dat


exp3b:
	@echo "Spusteni 150 predpripravenych simulaci. (Prumerny cas 10min na stroji merlin)"
	@echo "Program vypisuje ziskane data do souboru, ktery pouzijeme pro sestaveni grafu"
	@echo "Graf bude reprezentovat zavislost prumerneho toko v systemu na frekvenci prepinani svetel T"
	@echo "Specificke parametry exp: N = 10, D = 100, rho = 0.70, randomization p = 0.1"
	./sim 3b > vysledky/exp3b.dat

plot3:
	gnuplot gnuplot/exp3.gnu


exp4:
	./sim 4 > vysledky/exp4.dat

plot4:
	gnuplot gnuplot/exp4.gnu

exp5a:
	./sim 5a > vysledky/exp5a.dat

exp5b:
	./sim 5b > vysledky/exp5b.dat

plot5:
	gnuplot gnuplot/exp5.gnu

exp6a:
	./sim 6a > vysledky/exp6a.dat

exp6b:
	./sim 6b > vysledky/exp6b.dat
plot6:
	gnuplot gnuplot/exp6.gnu

exp7a:
	./sim 7a > vysledky/exp7a.dat

exp7b:
	./sim 7b > vysledky/exp7b.dat
plot7:
	gnuplot gnuplot/exp7.gnu



