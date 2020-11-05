PROJ=proj5

all:
	vlna   $(PROJ).tex
	latex  $(PROJ).tex
	latex  $(PROJ).tex
	
	dvips $(PROJ).dvi
	ps2pdf $(PROJ).ps


rm:
	rm -f  $(PROJ).blg $(PROJ).dvi $(PROJ).log $(PROJ).ps $(PROJ).aux $(PROJ).bbl 

zip:
	zip xvichs00-fit.zip Makefile proj5.tex
