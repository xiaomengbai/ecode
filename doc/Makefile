texsource=summary
bibsource=ref.bib
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