all: Writeup

Writeup: final.tex IEEEtran.cls IEEEtran.bst ref.bib
	latex final.tex -shell-escape
	bibtex final.aux
	latex final.tex -shell-escape
	latex final.tex -shell-escape
	dvips final.dvi
	ps2pdf final.ps
    
clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.toc
