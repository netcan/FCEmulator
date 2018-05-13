main.pdf: main.tex main.aux main.bbl
	xelatex -shell-escape -no-pdf $<
	# -8bit 正确处理tab
	xelatex -8bit -shell-escape $<

main.aux: main.tex
	xelatex -shell-escape -no-pdf $<

main.bbl: main.aux
	bibtex $<

clean:
	rm -f *.aux *.log *.swp *.lof *.lot *.out *.toc *.blg *.bbl *.xdv tex/*.aux
	rm -rf _minted-main
