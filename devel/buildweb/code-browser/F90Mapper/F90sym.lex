 /* $Id: F90sym.lex,v 1.1 1997/03/23 05:52:43 cnh Exp cnh $                                   */
 /* Lex analyser to produce a list of names */
 /* from a fixed form Fortran 90 program.   */
 /* Rules assume that *NO* Fortran 90       */
 /* keywords that the rules identify are    */
 /* used as variable names.                 */
%p 100000
%a 100000
%o 100000
%n 100000
%k 100000
%e 100000
%s LOADLINE
%option noyywrap

 /* Line and character no. counter */
 int Lno=1;int Cno=1;
NAME      [_a-zA-Z]+[_a-zA-Z0-9]*
INT       [0-9]*
EXPO      ([EeDd][+-]?{INT})
FLOAT     ([0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+){EXPO}?
CONT      ^[ ][ ][ ][ ][ ][^ \n].*
LAB       ^[1-9]....*
 /* Those wretched case insensitive keywords */
A [aA]
B [bB]
C [cC]
D [dD]
E [eE]
F [fF]
G [gG]
H [hH]
I [iI]
J [jJ]
K [kK]
L [lL]
M [mM]
N [nN]
O [oO]
P [pP]
Q [qQ]
R [rR]
S [sS]
T [tT]
U [uU]
V [vV]
W [wW]
X [xX]
Y [yY]
Z [zZ]

%{
#include "string.h"
#include "stdio.h"
#include "F90sym.tab.h" /* Tokens for yacc return values */
#include "GLOBALS.h"
int call1yylex = 0;
%}

%%
 if ( call1yylex == 0 ) { 
   BEGIN(LOADLINE);
   call1yylex = 1;
  }

^[CcDd].*  {Cno=Cno+F90symleng;                          /* 'C' or 'D' in column 1 F77 style comment         */
            strcat(currentLineHtml,"<I>");               /* echo italicised to HTML buffer                   */
            strcat(currentLineHtml,F90symtext);
            strcat(currentLineHtml,"</I>");
            return(OTHER);}

^[\*].*    {Cno=Cno+F90symleng;                          /* '*' in column 1                                  */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

'[^']*'    {Cno=Cno+F90symleng;                          /* String in ' quotes                               */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

\"[^\"]*\" {Cno=Cno+F90symleng;                          /* String in " quotes                               */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

!.*        {Cno=Cno+F90symleng;                          /* Inline quote                                     */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

\/\*.*     {Cno=Cno+F90symleng;                          /* Inline quote  ( C-style )                        */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

^[ \t]*    {Cno=Cno+F90symleng;                          /* Blank line                                       */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

{LAB}      {Cno=Cno+F90symleng;                          /* Col 1-5 label                                    */
            currentLineText[0]=(char)NULL;               /* Note the trick here:                             */
            sprintf(currentLineHtml,"<A NAME=%d_L></A>",Lno);
            strncat(currentLineHtml,F90symtext,5);       /* We have to match a whole line because otherwise  */
            strncat(currentLineText,F90symtext,5);       /* we loose our left context ^ to LOADLINE. We then */
            yyless(5);return(OTHER);}                    /* put back all but the first n chars after setting */
                                                         /* currentLineText.                                 */
                                                         /* I think the proper way to do this is via         */
                                                         /* a left context of <INITIAL>. This is working for */
                                                         /* #ifdef. Haven't tried it for LAB and CONT yet.   */

{CONT}     {Cno=Cno+F90symleng;                          /* Col 6 non-blank continuation                     */
            currentLineText[0]=(char)NULL;               /* Need to use same trick as col 1-5 label          */
            sprintf(currentLineHtml,"<A NAME=%d_L></A>",Lno);
            strncat(currentLineText,F90symtext,6);
            strncat(currentLineHtml,F90symtext,6);
            yyless(6);}                 

<INITIAL>#ifdef   |                                     /* ifdef CPP statement                              */
<INITIAL>#[ ]*ifdef  {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_IFDEF);}
<INITIAL>#undef   |                                     /* undef CPP statement                              */
<INITIAL>#[ ]*undef  {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_UNDEF);}
<INITIAL>#define  |                                     /* define CPP statement                             */
<INITIAL>#[ ]*define {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_DEFINE);}
<INITIAL>#elif    |                                     /* elif  CPP statement                              */
<INITIAL>#[ ]*elif   {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_ELIF);}
<INITIAL>#if      |                                     /* if    CPP statement                              */
<INITIAL>#[ ]*if     {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_IF   );}
<INITIAL>#ifndef   |                                    /* ifndef CPP statement                             */
<INITIAL>#[ ]*ifndef {Cno=Cno+F90symleng;
                      F90symlval.LineNo=Lno;
                      strcat(currentLineHtml,F90symtext);
                      return(CPP_IFNDEF);}

{INT}      {Cno=Cno+F90symleng;                          /* Integer                                          */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

{FLOAT}    {Cno=Cno+F90symleng;                          /* Floating point number                            */
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}   

^#ifndef.*      |                                         /* C preprocessor controls                          */
^#[ ]*endif.*   |
^#undef.*       |
^#define.*      |
^#elif          |
^#if            |
^#[ ]*include.* |
^#else.*   {Cno=Cno+F90symleng;
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}

[ \t\r]    {Cno=Cno+F90symleng;                          /* Blank space                                      */
            strcat(currentLineHtml,F90symtext);}    

\(         {Cno=Cno+F90symleng;                          /* ( bracket                                        */
            strcat(currentLineHtml,"<FONT SIZE=5><B>");  /* echo bold to HTML buffer                         */
            strcat(currentLineHtml,F90symtext);
            strcat(currentLineHtml,"</FONT></B>");
            return(OTHER);}     

\)         {Cno=Cno+F90symleng;                          /* ) bracket                                        */
            strcat(currentLineHtml,"<FONT SIZE=5><B>");  /* echo bold to HTML buffer                         */
            strcat(currentLineHtml,F90symtext);
            strcat(currentLineHtml,"</FONT></B>");
            return(OTHER);}     

\, |                                                     /* Various punctutation symbols or operators        */
\. |                                                     /* which are just echoed to the HTML buffer.        */
\: |
\= |
\+ |
\- |
\*         {Cno=Cno+F90symleng;                         
            strcat(currentLineHtml,F90symtext);
            return(OTHER);}     
\/         {Cno=Cno+F90symleng;
            strcat(currentLineHtml,F90symtext);
            return(FSLASH);} 

{A}{B}{S}                   |                            /* Language keywords which are just echoed to      */
{A}{L}{L}                   |                            /* HTML buffer. ( Alphabetical order )             */
{A}{N}{D}                   |
{A}{N}{Y}                   |
{C}{H}{A}{R}{A}{C}{T}{E}{R} |
{C}{L}{O}{S}{E}             |
{C}{O}{N}{T}{I}{N}{U}{E}    |
{C}{O}{S}                   | 
{C}{O}{U}{N}{T}             |
{C}{S}{H}{I}{F}{T}          |
{D}{A}{T}{A}                |
{D}{I}{M}                   |
{E}{N}{D}{W}{H}{E}{R}{E}    |
{E}{Q}                      |
{F}{A}{L}{S}{E}             |
{F}{I}{L}{E}                |
{F}{L}{O}{A}{T}             |
{F}{M}{T}                   |
{F}{O}{R}{A}{L}{L}          |
{F}{O}{R}{M}{A}{T}.*        |
{F}{U}{N}{C}{T}{I}{O}{N}    |
{G}{E}                      |
{G}{O}{T}{O}                |
[ ]*\.{G}{T}[ ]*\.          |  /* Unfortunately we chose to call a variable gT! */
{I}{M}{P}{L}{I}{C}{I}{T}    |
{I}{N}{C}{L}{U}{D}{E}       |
{I}{N}{D}{E}{X}             |
{I}{N}{T}                   |
{I}{N}{T}{E}{G}{E}{R}       |
{I}{O}{S}{T}{A}{T}          |
{L}{E}                      |
{L}{E}{N}                   |
{L}{O}{G}{I}{C}{A}{L}       |
{L}{T}                      |
{M}{A}{S}{K}                |
{M}{A}{X}                   |
{M}{A}{X}{V}{A}{L}          |
{M}{I}{N}                   |
{M}{I}{N}{V}{A}{L}          |
{M}{O}{D}                   |
{N}{E}                      |
{N}{I}{N}{T}                |
{N}{O}{N}{E}                |
{N}{O}{T}                   |
{O}{P}{E}{N}                |
{O}{R}                      |
{P}{A}{R}{A}{M}{E}{T}{E}{R} |
{P}{R}{I}{N}{T}             |
{P}{R}{O}{G}{R}{A}{M}       |
{R}{E}{A}{D}                |
{R}{E}{A}{L}                |
{R}{E}{T}{U}{R}{N}          |
{R}{E}{W}{I}{N}{D}          |
{S}{A}{V}{E}                |
{S}{H}{I}{F}{T}             |
{S}{I}{N}                   |
{S}{Q}{R}{T}                |
{S}{T}{A}{T}{U}{S}          |
{S}{T}{O}{P}                |
{S}{U}{M}                   |
{T}{A}{N}                   |
{T}{H}{E}{N}                |
{T}{R}{U}{E}                |
{U}{N}{I}{T}                |
{W}{R}{I}{T}{E}             {Cno=Cno+F90symleng;
                             strcat(currentLineHtml,F90symtext);
                             return(OTHER);}

{D}{O}                      |                            /* Language keywords which are just echoed BOLD    */
{E}{L}{S}{E}                |                            /* to HTML buffer. ( Alphabetical order )          */
{E}{L}{S}{E}{I}{F}          |
{E}{L}{S}{E}{W}{H}{E}{R}{E} |
{E}{N}{D}{D}{O}             |
{E}{N}{D}{I}{F}             |
{I}{F}                      |
{W}{H}{E}{R}{E}             {Cno=Cno+F90symleng;
                              strcat(currentLineHtml,"<B>");
                              strcat(currentLineHtml,F90symtext);
                              strcat(currentLineHtml,"</B>");
                              return(OTHER);}

{E}{N}{D}=                     {Cno=Cno+F90symleng;                  /* END= in I/O statement                   */
                                strcat(currentLineHtml,"<B>");
                                strcat(currentLineHtml,F90symtext);
                                strcat(currentLineHtml,"</B>");
                                return(OTHER);}
{C}{A}{L}{L}                   {Cno=Cno+F90symleng;                  /* CALL returns "CALL" token to parser */
                                F90symlval.LineNo=Lno;
                                strcat(currentLineHtml,"<U><B>");
                                strcat(currentLineHtml,F90symtext);
                                strcat(currentLineHtml,"</B></U>");
                                return(CALL);}
{E}{N}{D}                      {Cno=Cno+F90symleng;                  /* END causes horizontal line to be        */
                                strcat(currentLineHtml,F90symtext);  /* to HTML buffer.                         */
                                strcat(currentLineHtml,"<HR>");      /* Exclude END= because we don't want      */
                                strcat(currentLineHtml,"<HR>");      /* to match END= clauses in I/O statements */
                                strcat(currentLineHtml,"<HR>");
                                return(OTHER);}
{E}{X}{T}{E}{R}{N}{A}{L}       {Cno=Cno+F90symleng;                  /* EXTERNAL returns "EXTERNAL" token to*/
                                F90symlval.LineNo=Lno;               /* parser.                             */
                                strcat(currentLineHtml,F90symtext);
                                return(EXTERNAL);}
{S}{U}{B}{R}{O}{U}{T}{I}{N}{E} {Cno=Cno+F90symleng;                  /* Write SUBROUTINE to HTML buffer bold*/
                                F90symlval.LineNo=Lno;               /* and with large font                 */
                                strcat(currentLineHtml,"<FONT SIZE=8><B>");
                                strcat(currentLineHtml,F90symtext);
                                strcat(currentLineHtml,"</FONT></B>");
                                return(SUBROUTINE);}                 /* Return "SUBROUTINE" token to parser */
{N}{A}{M}{E}{L}{I}{S}{T}       {Cno=Cno+F90symleng;
                                strcat(currentLineHtml,F90symtext);
                                return(NAMELIST);}

{NAME}     {Cno=Cno+F90symleng;                                      /* At last! A variable name. This is     */
            F90symlval.symbolName=strdup(F90symtext);                /* written to HTML buffer by the parser  */
            return(NAME);}                                           /* code cos we need to know the HREF     */

<LOADLINE>.* { if ( currentLineText[0] == (char)NULL || 
                    currentLineText[0] == '#'        )                      /* Special trick to create a      */
                {  /* Not a continuation or label */                        /* string containing the current  */
                 strncpy(currentLineText,F90symtext,currentLineBufSize-1);  /* line.                          */
                 sprintf(currentLineHtml,"<A NAME=%d_L></A>",Lno);
                 newLineNotify();
                 newStatementNotify();                                      /* Call function to clear context */
                }                                                           /* related to previous statement  */
               else 
                {  /* Continuation or label       */
                 newLineNotify();
                 strncat(currentLineText,F90symtext,currentLineBufSize-7);
                }
               yyless(0);
               BEGIN(INITIAL);
             }

\n\r |
\n           {                                                                   /* Newline                       */
              F90db_Newline();                                                   /* Write out HTML buffer         */
              BEGIN(LOADLINE);
              ++Lno;Cno=0;
              currentLineText[0]=(char)NULL;                                     /* Clear current line buffer     */
              currentLineHtml[0]=(char)NULL;}                                    /* Clear HTML buffer             */

.            {printf(" Unclassified symbol \"%s\" @ line %d\n",F90symtext,Lno);  /* Something unrecognised        */
              Cno=Cno+F90symleng;}                                               /* This should not happen for    */
                                                                                 /* legitimate programs.          */
