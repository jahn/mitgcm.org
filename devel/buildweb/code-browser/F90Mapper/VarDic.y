/* $Id: */

/* Data dictionary -> HTML table translator */
%{
#include <stdio.h>
#include <string.h>
#include "DD.h"
#include "GLOBALS.h"
%}

%union {
   int     LineNo;
   char    *stringPtr;
}

%token <LineNo> HREF
%token <LineNo> TEXT
%token <LineNo> UNITS
%token <LineNo> NOTES
%token <stringPtr> NAME
%token <stringPtr> STRING
%type  <stringPtr> stringList

%%

input:  /* empty */
      | input recordDefinition
;
recordDefinition:  name '{' tableEntries '}'  {addRecord();}
                 | name                       {addRecord();}
;
tableEntries:  tableEntries tableEntry        {}
             | tableEntry                     {}
;
tableEntry:  HREF  '=' stringList ';'     {curHref      = strdup($3);}
           | TEXT  '=' stringList ';'     {fprintf(stdout,"TEXT %s found\n",$3);
                                           curText      = strdup($3);
                                           fprintf(stdout,"TEXT %s found\n",$3);
                                           fflush(stdout);}
           | UNITS '=' stringList ';'     {curUnits     = strdup($3);}
           | NOTES '=' stringList ';'     {curFootNotes = strdup($3);}
           | error ';'        
;
name: NAME {curName = strdup($1);
            fprintf(stdout,"NAME %s found\n",curName);fflush(stdout);
           } 
;
stringList:  stringList STRING {$$=strjoin($1,$2); }
           | STRING            {$$=$1;             }
;
%%
