 /* $Id: VarDic.lex,v 1.2 1997/03/22 20:58:46 cnh Exp $ */
 /* Lex analyser to process variable database into HTML table */
%p 100000
%a 100000
%o 100000
%n 100000
%k 100000
%e 100000
%option noyywrap

/* Matches variable names */
NAME      [_a-zA-Z]+[_a-zA-Z0-9\./]*

%{
#include <stdio.h>
#include <string.h>
#include "VarDic.tab.h"
#include "GLOBALS.h"

/* DEBUGGING aids */
#define DP(a)
#define DP(a) (printf("\"%s\" (%d,%d)\n",a,Lno,Cno))
#define RETURN(a) return(a)
/* #define RETURN(a) printf("Token %d\n",a);return(a)   */
%}
%%
^#.*   {DP("COMMENT");}
HREF   {DP("HREF"); Cno=Cno+VarDicleng; RETURN(HREF);}
TEXT   {DP("TEXT"); Cno=Cno+VarDicleng; RETURN(TEXT);}
UNITS  {DP("UNITS"); Cno=Cno+VarDicleng; RETURN(UNITS);}
NOTES  {DP("NOTES"); Cno=Cno+VarDicleng; RETURN(NOTES);}
{NAME} {DP("NAME"); Cno=Cno+VarDicleng; 
        VarDiclval.stringPtr=strdup(VarDictext);  
        RETURN(NAME);}
[ ]    {DP("SPACE"); Cno=Cno+VarDicleng; }
\{     {DP("{"); Cno=Cno+VarDicleng; RETURN('{');}
\}     {DP("}"); Cno=Cno+VarDicleng; RETURN('}');}
;      {DP(";"); Cno=Cno+VarDicleng; RETURN(';');}
=      {DP("="); Cno=Cno+VarDicleng; RETURN('=');}

\"([^\"\n\\\\"]|\\.)*\" {DP("STRING"); Cno=Cno+VarDicleng; 
                         VarDiclval.stringPtr=strcopy(VarDictext+1);
                         VarDiclval.stringPtr[strlen(VarDiclval.stringPtr)-1]='\0';
                         RETURN(STRING);}

\"([^\"\n\\\\"]|\\.)*$  {DP("UNTERMINATED STRING");
                         Cno=Cno+VarDicleng;
                         fprintf(stderr,
                         "Error: unterminated string. <%s> Line %d Column %d\n",
                         currentFile,Lno,Cno);
                         fprintf(stderr,"==> %s\n",VarDictext);
                         }


\n     {DP("\\n");
        ++Lno;Cno=1;
       }

.    {DP("UNMATCHED");
      Cno=Cno+VarDicleng;
     }
