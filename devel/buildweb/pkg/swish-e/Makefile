# Makefile for swish-e 2.1
#
# Bas Meijer, bas@antraciet.com 11 Dec 2000

# $Id: Makefile.in,v 1.12 2001/10/06 05:31:40 whmoseley Exp $

PROJECT     = swish-e
VERSION     = 2.2rc1

SHELL       = /bin/sh
srcdir      = .

prefix      = /u/httpd/html/devel/buildweb
exec_prefix = ${prefix}
bindir      = $(prefix)/bin
mandir      = $(prefix)/man/man1
PERL        = /usr/bin/perl
libdir      = ${exec_prefix}/lib
 


#-- for making distribution --- (should let ./configure find these programs)
DIST_DEFAULT= tardist
TAR         = tar
TARFLAGS    = cvf
COMPRESS    = gzip
RM_RF       = rm -rf
DISTVNAME   = $(PROJECT)-$(VERSION)
SUFFIX      = gz

#-- for building code, testing, and installing --

all:	swish-e swish-search
	@echo -e '\nNow make test\n'
	
swish-e:	
	@echo 'making swish-e'
	(cd src; $(MAKE))
	@echo 'swish-e done'

swish-search: swish-e
	@echo 'making swish-search'
	cp -f src/swish-e src/swish-search
	@echo 'swish-search done'


perllib:
	@echo 'making Perl library for Swish-E'
	(cd perl;$(PERL) Makefile.PL PREFIX=$(libdir);$(MAKE)) 

test:
	(cd src; $(MAKE) test)

install:
	cp src/swish-e $(bindir)/swish-e

install-lib:
	cp src/libswish-e.a $(libdir)/libswish-e.a


#-- Cleanup tags --

clean:
	-(cd src; $(MAKE) clean)
	-(cd perl; $(MAKE) clean)
	-(cd doc; $(MAKE) clean)

realclean: clean
	rm -f config.cache config.log config.status Makefile
	-(cd src; $(MAKE) realclean)
	-(cd perl; $(MAKE) realclean)
	-(cd doc; $(MAKE) realclean)


#-- Documentation --
docs:
	$(RM_RF) html
	(cd doc; $(MAKE) all)
	
pdf:
	(cd doc; $(MAKE) pdf)

install-man:
	(cd doc; $(MAKE) install-man)

#---- Distribution --------

distclean :: realclean distcheck

distcheck :
	$(PERL) -MExtUtils::Manifest=fullcheck \
		-e fullcheck
        
skipcheck :
	$(PERL) -MExtUtils::Manifest=skipcheck \
		-e skipcheck

distdir :
	$(RM_RF) $(DISTVNAME)
	$(PERL)  -MExtUtils::Manifest=manicopy,maniread \
		-e "manicopy(maniread(),'$(DISTVNAME)', 'best');"

$(DISTVNAME).tar$(SUFFIX) : distdir
	$(TAR) $(TARFLAGS) $(DISTVNAME).tar $(DISTVNAME)
	$(RM_RF) $(DISTVNAME)
	$(COMPRESS) $(DISTVNAME).tar

tardist : $(DISTVNAME).tar$(SUFFIX)

dist : docs $(DIST_DEFAULT)

disttoday : docs distdir
	$(TAR) $(TARFLAGS) $(DISTVNAME)-`/bin/date +'%Y-%m-%d'`.tar $(DISTVNAME)
	$(RM_RF) $(DISTVNAME)
	$(COMPRESS) $(DISTVNAME)-`/bin/date +'%Y-%m-%d'`.tar


