a.out: Getcomm.y Getcomm.lex GLOBALS.h
	bison -p Getcomm -d -t Getcomm.y
	flex -PGetcomm Getcomm.lex
	cc lex.Getcomm.c Getcomm.tab.c
clean:
	\rm lex.Getcomm.c Getcomm.tab.h Getcomm.tab.c

# DO NOT DELETE
