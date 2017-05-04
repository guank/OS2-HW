all: Writeup Concurrency

Writeup: project2_writeup.tex IEEEtran.cls
	latex project2_writeup.tex -shell-escape
	dvips project2_writeup.dvi
	ps2pdf project2_writeup.ps

Concurrency: conc2.c
	gcc conc2.c -o conc2 -std=c99 -g -pthread
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
