all: Writeup

Writeup: writing2.tex IEEEtran.cls IEEEtran.bst ref.bib
	latex writing2.tex -shell-escape
	bibtex writing2.aux
	latex writing2.tex -shell-escape
	latex writing2.tex -shell-escape
	dvips writing2.dvi
	ps2pdf writing2.ps
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
