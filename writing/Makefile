all: Writeup

Writeup: writing1.tex IEEEtran.cls IEEEtran.bst ref.bib
	latex writing1.tex -shell-escape
	bibtex writing1.aux
	latex writing1.tex -shell-escape
	latex writing1.tex -shell-escape
	dvips writing1.dvi
    ps2pdf writing1.ps
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
