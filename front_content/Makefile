#  $ Header: $
#
#  Makefile for the "front-end" of the mitgcm.org domain.
#


all:
	-rm -rf html
	export CVSROOT=:ext:@auden.lcs.mit.edu:/u/u3/gcmpack \
  export CVS_RSH=ssh && cvs -q up
	./parse_emails
	./make_summary
	./Assemble.pl

install:
	@make all
	scp html/*.html mitgcm.org:/u/u0/httpd/html/

tidy:
	tidy -i -wrap 80 -clean -asxml template_003.xml > template_004.xml

clean:
	-rm -rf html ptmp


