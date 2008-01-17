#  $Id: Makefile,v 1.1.1.1 2002/09/20 19:47:31 adcroft Exp $
#  Makefile for building f90mapper.
#  f90mapper creates an HTML map of a set of F90 source
#  files. Tool reads source code and variable dictionary 
#  to produce a hyperlinked static analysis of the code.
#
# Tools used to build the mapper.
LEX=flex
CC=gcc -g
YACC=bison
# Executable produced
target=f90mapper
# Names of components
# 1. Variable dictionary parser.
VD=VarDic
# 2. Data dictionary manipulation routines.
DD=dd
# 3. F90 symbol parser.
FS=F90sym
# 4. F90 tree builder
DB=F90db

$(target): lex.$(VD).o $(VD).tab.o \
           lex.$(FS).o $(FS).tab.o \
           main.o $(DD).o $(DB).o makeOutputDirectories.o \
           base36.o fd.o html_write.o
	$(CC) -o $(target) \
                 lex.$(VD).o $(VD).tab.o \
                 lex.$(FS).o $(FS).tab.o \
                 main.o $(DD).o $(DB).o  makeOutputDirectories.o  \
                 base36.o fd.o html_write.o  \
                 -ll
.c.o:
	$(CC) -c -g $<

lex.$(VD).c: $(VD).lex $(VD).tab.h
	$(LEX) -P$(VD) $(VD).lex 

$(VD).tab.h $(VD).tab.c: $(VD).y
	$(YACC) -p $(VD) -d -t $(VD).y

lex.$(FS).c: $(FS).lex $(FS).tab.h
	$(LEX) -P$(FS) $(FS).lex 

$(FS).tab.h $(FS).tab.c: $(FS).y
	$(YACC) -p $(FS) -d -t $(FS).y

clean:
	rm *.o \
        lex.$(VD).c $(VD).tab.h $(VD).tab.c \
        lex.$(FS).c $(FS).tab.h $(FS).tab.c
# DO NOT DELETE
