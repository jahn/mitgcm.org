
all:
	-rm -rf html
	./Assemble.pl

install:
	@make all
	scp html/*.html mitgcm.org:/u/u0/httpd/html/

tidy:
	tidy -i -wrap 80 -clean -asxml template_003.xml > template_004.xml

clean:
	-rm -rf html
