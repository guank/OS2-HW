all: Writeup Concurrency

Writeup: cs444_project1_writeup.tex IEEEtran.cls
	latex cs444_project1_writeup.tex -shell-escape
	dvips cs444_project1_writeup.dvi
	ps2pdf cs444_project1_writeup.ps

Concurrency: concurrency1.c
    gcc concurrency1.c -o concurrency1 -std=c99 -g -pthread
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
