#  $Header: /u/gcmpack/mitgcm.org/front_content/Makefile,v 1.20 2018/01/14 18:03:16 jmc Exp $
#
#  Makefile for the "front-end" of the mitgcm.org domain.

all:
	-rm -rf html
#	./parse_emails -u /u/edhill/local/bin/munpack
	./make_summary
	./Assemble.pl

install:
	@make all
	chmod 664 html/*.html
	/bin/cp -f html/*.html /u/u0/httpd/html/public/
	#cp `ls html/*.html | grep -v 'html/news.html'` /u/u0/httpd/html/public/

tidy:
	tidy -i -wrap 80 -clean -asxml template_003.xml > template_004.xml

clean:
	-rm -rf html ptmp

