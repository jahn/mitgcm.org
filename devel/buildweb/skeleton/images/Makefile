BGCOLOR = black
COLOR   = white
MOCOLOR = blue
FONT    = "-*-helvetica-medium-r-*-*-20-*-*-*-*-*-iso8859-*"
HEIGHT  = 34

all: nhome.gif nhome_mo.gif ncontactus.gif ncontactus_mo.gif nsitemap.gif nsitemap_mo.gif

nhome.gif: Makefile
	ppmmake $(BGCOLOR) 116 $(HEIGHT) | ppmtogif | convert -fill $(COLOR) -font $(FONT) -draw "text 6,22 'MITgcm.org'" - $@

nhome_mo.gif: Makefile
	ppmmake $(BGCOLOR) 116 $(HEIGHT) | ppmtogif | convert -fill $(MOCOLOR) -font $(FONT) -draw "text 6,22 'MITgcm.org'" - $@

ncontactus.gif: Makefile
	ppmmake $(BGCOLOR) 108 $(HEIGHT) | ppmtogif | convert -fill $(COLOR) -font $(FONT) -draw "text 6,22 'Contact Us'" - $@

ncontactus_mo.gif: Makefile
	ppmmake $(BGCOLOR) 108 $(HEIGHT) | ppmtogif | convert -fill $(MOCOLOR) -font $(FONT) -draw "text 6,22 'Contact Us'" - $@

nsitemap.gif: Makefile
	ppmmake $(BGCOLOR) 97 $(HEIGHT) | ppmtogif | convert -fill $(COLOR) -font $(FONT) -draw "text 6,22 'Site Index'" - $@

nsitemap_mo.gif: Makefile
	ppmmake $(BGCOLOR) 97 $(HEIGHT) | ppmtogif | convert -fill $(MOCOLOR) -font $(FONT) -draw "text 6,22 'Site Index'" - $@
