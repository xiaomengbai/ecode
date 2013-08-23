texsource=summary
bibsource=ref.bib
cleanfile=*.dvi *.pdf *.log *.aux *.blg *.bbl
dvi: $(texsource).tex $(bibsource) 
	latex $(texsource)
	bibtex $(texsource)
	latex $(texsource)
	latex $(texsource)

pdf: $(texsource).tex $(bibsource)
	pdflatex $(texsource)
	bibtex $(texsource)
	pdflatex $(texsource)
	pdflatex $(texsource)
clean:
	-rm $(cleanfile)