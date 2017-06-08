all: Writeup

Writeup: writing3.tex IEEEtran.cls IEEEtran.bst ref.bib
	latex writing3.tex -shell-escape
	bibtex writing3.aux
	latex writing3.tex -shell-escape
	latex writing3.tex -shell-escape
	dvips writing3.dvi
	ps2pdf writing3.ps
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
