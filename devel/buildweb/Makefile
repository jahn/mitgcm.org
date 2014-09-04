# Paths

SHELL       = /bin/bash
#eh3  TARGETDIR   = /u/httpd/html/devel/sealion
TARGETDIR   = ../../../scratch/dev_docs
BUILDWEBDIR = $(PWD)

SKELETON    = $(BUILDWEBDIR)/skeleton
#eh3 LATEXROOT   = $(BUILDWEBDIR)/latex
LATEXROOT   = ../../../manual
VDBROOT     = $(BUILDWEBDIR)/vdb
BINDIR      = $(BUILDWEBDIR)/bin
PFILESDIR   = $(BUILDWEBDIR)/program_files
#jmc: try relative path to speed-up copies
SKELETON    = ./skeleton
VDBROOT     = ./vdb

all: Templates InstallSkeleton InstallLatex InstallPDF InstallOLF InstallVDB SiteMap Search2 Bonniefy Index
All: pdf Latex VDB all
quick: Templates InstallSkeleton InstallLatex InstallPDF SiteMap Search2 Bonniefy Index

Templates:
	cd templates; make

InstallSkeleton:
	mkdir -p $(TARGETDIR)
	cp -rp $(SKELETON)/* $(TARGETDIR)
	chmod -R u+w $(TARGETDIR)

InstallLatex:
	mkdir -p $(TARGETDIR)/online_documents
	cp -rp $(LATEXROOT)/manual/* $(TARGETDIR)/online_documents/

InstallPDF:
	mkdir -p $(TARGETDIR)/online_documents
	cp -p $(LATEXROOT)/manual.p* $(TARGETDIR)/online_documents/

InstallOLF:
	mkdir -p $(TARGETDIR)/on-line-figs
	cp -rp $(LATEXROOT)/on-line-figs/* $(TARGETDIR)/on-line-figs/

#jmc: to speed up, use a "mv" of dir VDBROOT instead of a copy:
#     but this prevents to do multiple "make quick" after a "make VDB"
InstallVDB:
	mkdir -p $(TARGETDIR)/code_reference
	#cp -rp $(VDBROOT) $(TARGETDIR)/code_reference/
	mv $(VDBROOT) $(TARGETDIR)/code_reference/

Bonniefy: $(TARGETDIR)
	chmod -R u+w $(TARGETDIR)
	$(BINDIR)/bonniefy.pl --targetdir=$(TARGETDIR) --homedir=$(BUILDWEBDIR)

Index: $(TARGETDIR)/home_page/frontpage.html
	cat $(TARGETDIR)/home_page/frontpage.html \
	 | sed 's:\.\./::' > $(TARGETDIR)/index.html

SiteMap:
	echo $(SHELL)
	mkdir -p $(TARGETDIR)/sitemap
	cd $(TARGETDIR)/sitemap; $(BINDIR)/makesitemap ../online_documents > index.html

Search:
#	cd $(TARGETDIR); $(BINDIR)/swish-e -c $(PFILESDIR)/Swish-e.conf
Search2:
#	cd $(TARGETDIR); $(BINDIR)/swish-e -c $(PFILESDIR)/Swish-e.conf \
#		-S prog -i $(BINDIR)/DirTree.pl

Latex: 
	cd $(LATEXROOT); make html

pdf:
	cd $(LATEXROOT); make ps pdf ps.gz

VDB:
	cd code-browser; make dev

Clean:
	cd templates; make clean
	cd $(LATEXROOT); make Clean

TestSearch:
	$(BINDIR)/swish-e -f $(TARGETDIR)/index.swish-e -w adcroft

