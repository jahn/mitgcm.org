# $Header:  $

TARGS = about.htm browse.htm documentation.htm install.htm tutorials.htm search.htm sitemap.htm

HEADERS = bits/header.html bits/topbar.html
FOOTERS = bits/footer.html
COMMONFILES = $(HEADERS) $(FOOTERS)

all: $(TARGS)

about.htm: bits/about_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
browse.htm: bits/browse_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
documentation.htm: bits/documentation_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
install.htm: bits/install_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
tutorials.htm: bits/tutorials_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
search.htm: bits/search_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@
sitemap.htm: bits/about_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) | replace.sh > $@

clean:
	rm -f $(TARGS)
