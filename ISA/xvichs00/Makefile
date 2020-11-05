all: testovac

testovac: main.cpp
	g++ main.cpp -std=c++11 -lpthread -o testovac

rm:
	rm -f testovac

pr1: testovac
	sudo ./testovac www.root.cz -v

man:	testovac.1
	man -l testovac.1

zip:	testovac.1 main.cpp Makefile manual.pdf
	tar -cvf xvichs00.tar testovac.1 main.cpp Makefile manual.pdf
