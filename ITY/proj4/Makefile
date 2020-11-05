PROJ=proj4

all:
	vlna   $(PROJ).tex
	latex  $(PROJ).tex
	bibtex $(PROJ).aux
	latex  $(PROJ).tex
	latex  $(PROJ).tex
	
	dvips -t a4 $(PROJ).dvi
	ps2pdf $(PROJ).ps


rm:
	rm -f  $(PROJ).blg $(PROJ).dvi $(PROJ).log $(PROJ).ps $(PROJ).aux $(PROJ).bbl 
