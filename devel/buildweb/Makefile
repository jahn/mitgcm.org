# Paths

TARGETDIR   = /u/httpd/html/devel/sealion
BUILDWEBDIR = $(PWD)

SKELETON    = $(BUILDWEBDIR)/skeleton
LATEXROOT   = $(BUILDWEBDIR)/latex
VDBROOT     = $(BUILDWEBDIR)/vdb
BINDIR      = $(BUILDWEBDIR)/bin
PFILESDIR   = $(BUILDWEBDIR)/program_files

all: Templates InstallSkeleton Search2 Bonniefy Index
All: Latex VDB all

Templates:
	cd templates; make

InstallSkeleton:
	rm -rf $(TARGETDIR)
	cp -rp $(SKELETON) $(TARGETDIR)
	cp -rp $(LATEXROOT)/manual/* $(TARGETDIR)/online_documents/
	cp -rp $(LATEXROOT)/on-line-figs/* $(TARGETDIR)/on-line-figs/
	cp -rp $(VDBROOT) $(TARGETDIR)/code_reference/

Bonniefy: $(TARGETDIR)
	$(BINDIR)/bonniefy.pl --targetdir=$(TARGETDIR) --homedir=$(HOMEDIR)

Index: $(TARGETDIR)/home_page/frontpage.html
	cat $(TARGETDIR)/home_page/frontpage.html \
	 | sed 's:\.\./::' > $(TARGETDIR)/index.html

Search:
	cd $(TARGETDIR); $(BINDIR)/swish-e -c $(PFILESDIR)/Swish-e.conf

Search2:
	cd $(TARGETDIR); $(BINDIR)/swish-e -c $(PFILESDIR)/Swish-e.conf \
		-S prog -i $(BINDIR)/DirTree.pl

Latex: latex
	cd latex; make html

VDB:
	cd code-browser; source README

Clean:
	cd templates; make clean
	cd latex; make Clean

TestSearch:
	$(BINDIR)/swish-e -f $(TARGETDIR)/index.swish-e -w adcroft

latex:
	cvs co -d latex manual -d -P
