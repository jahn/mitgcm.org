#  $Header: /u/gcmpack/mitgcm.org/front_content/Makefile,v 1.15 2008/02/26 21:42:12 jmc Exp $
#
#  Makefile for the "front-end" of the mitgcm.org domain.
#


all:
	-rm -rf html
#	./parse_emails -u /u/edhill/local/bin/munpack
	./make_summary
	./Assemble.pl

install:
	@make all
	cp html/testing.html /u/u0/httpd/html/public/
	#cp html/*.html cmi_redir.php /u/u0/httpd/html/

tidy:
	tidy -i -wrap 80 -clean -asxml template_003.xml > template_004.xml

clean:
	-rm -rf html ptmp


