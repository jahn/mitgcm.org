default:
	@echo "Targets"
	@echo " make all   - everything in order"
	@echo " make html  - generate LaTeX and HTML manual"
	@echo " make files - PS and PDF files of manual"
	@echo " make links - create symbolic links into manual for front end"

all:
	make html 
	make files
	make links

html:
	cd latex; make tex
	cd latex; make html

files:
	cd latex; make pdf
	ln -sf latex/manual.pdf manual.pdf
	cd latex; make ps.gz
	ln -sf latex/manual.ps.gz manual.ps.gz

links:
	ln -sf latex/manual manual
	ln -sf latex/on-line-figs on-line-figs
	./generate_links
