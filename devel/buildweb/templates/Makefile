TARGS = about.htm browse.htm documentation.htm install.htm tutorials.htm search.htm

HEADERS = bits/header.html bits/topbar.html
FOOTERS = bits/footer.html
COMMONFILES = $(HEADERS) $(FOOTERS)

all: $(TARGS)

about.htm: bits/about_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@
browse.htm: bits/browse_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@
documentation.htm: bits/documentation_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@
install.htm: bits/install_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@
tutorials.htm: bits/tutorials_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@
search.htm: bits/search_side.htm $(COMMONFILES)
	cat $(HEADERS) $<  $(FOOTERS) > $@

clean:
	rm -f $(TARGS)
