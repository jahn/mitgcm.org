/*

 Simple lexical analyser for finding and extracting "definition statements". This
 analyser finds "definition statements" within Fortran
 comment blocks. An analyser to find "definition statements"
 in other languages would look very similar.
 A "definition statement" consists of a name_list
 block and a descriptioin block, separated by a delimiter.
 A single definition statement can have formats of the style

 name_list          :: description

  -or-

 name_list,         :: description
 name_list          :: description
                    :: description

  -or-

 name_list,         :: description
 name_list,         :: description
 name_list          ::

  -or-

  name_list         :: description
                    :: description

 The name_list entries to the left of the ' ::' delimiter are the definition statements 
 name_list block. The description entries to the right of the ' ::' delimiter are the
 definition statements description block.

 description is text up to a new line.
 name_list is a list of one or more names, separated by white space or other
 punctuation (-,+,*,etc....). A name is a variable name, CPP symbol, procedure name 
 etc...
 The :: delimiter must have at least one blank preceeding it. This is required so
 that the C++ :: syntax for delimiting objects in a class is not interpreted as a definition 
 statement.

 These statements are used in structured commentary to provide 
 definition documentation of variables, procedures etc... that can be extracted 
 automatically for use in code synopsis documents and for use in code analysis.

 This lexer works with Fortran [CcDd] in column one comments.
 A very similar lexer could be used for C and C++ style one-line
 and multi-live comments, as well as for Fortran ! comments.

 Grammar that has to be recognised and processed is

 1. COMMENT_STARTED name_list        DESCRIP_DELIM descrip_text EOL   
 2. COMMENT_STARTED name_list  COMMA DESCRIP_DELIM descrip_text EOL
 3. COMMENT_STARTED DESCRIP_DELIM descrip_text EOL
 4. COMMENT_STARTED name_list EOL
 5. COMMENT_STARTED name_list COMMA EOL
 6. COMMENT_STARTED EOL

 1. This is recognised as a terminal record defining a list of variables
    associated with accumulated descrip_text. The description can continue
    over subsequent lines, but if there is a name_list on a subsequent line
    it will be assumed to be the start of another definition statement.

 2. This is recognised as an intermediate record defining a list of variables
    associated with accumulated descrip_text. The name_list COMMA syntax is
    the only way to indicate that a name_list spanning several lines in a file
    shares a single description block.

 3. This is recognised as a continued line of description for the
    immediately preceeding variable list.

 4.,5.,6. These are recognised as valid grammr and are ignored.

 NAME_LIST and DESCRIP_TEXT pass strings to the parser. Other states
 just set line number, character number and token id.

*/

/* Set limits for internal tables so they don't overflow */
%p 100000
%o 100000
%n 100000
%k 100000
%e 100000
%x COMMENT_STARTED
%x IN_DESCRIP
%x NOT_A_COMMENT
%option noyywrap

/* Things we will match */
NAME    [_a-zA-Z]+[_a-zA-Z0-9.]*
DSPLIT  [ ]*::
COMMA   ,

%{
#define DP(a)
#include "string.h"
#include "stdio.h"
#include "Getcomm.tab.h"
/* Line and character no. counter */
int Lno=1;int Cno=1;
int call1Getcommlex = 0;
#include "GLOBALS.h"
%}


%%

 if ( call1Getcommlex == 0 ) {
   BEGIN(NOT_A_COMMENT);
   call1Getcommlex = 1;
   namecount=0;
   descriptcount=0;
   name_open=0;
   descript_open=0;
  }


<NOT_A_COMMENT>^[CcDd]  {              /* A comment starts                   */
          Cno=Cno+Getcommleng;         /* In Fortran this is a C in column 1 */
          BEGIN(COMMENT_STARTED);      /* Could also do C-style and ! style  */
          DP(fprintf(stdout,"<COMMENT_START>\n");)
          return(COMMENT_START);
         }

<COMMENT_STARTED>{NAME} {                                    /* In the start of a comment any word   */
                    Cno=Cno+Getcommleng;                     /* could be a name part of a definition */
                    DP(fprintf(stdout,"<NAME>\n");)             /* statement..                          */
                    DP(fprintf(stdout,"%s",Getcommtext);)       /* ACTION:                      */
                    DP(fprintf(stdout,"\n<\\NAME>\n");)         /*  Return NAME token and value */
                    /* strdup name onto stack; increment name stack counter */
                    namearr[namecount]=strdup(Getcommtext);
                    ++namecount; if ( namecount == MAX_NAMEARR ) {
                     fflush(stdout);
                     fprintf(stderr,"namecount == %d, need to increase MAX_NAMEARR in Getcomm.lex\n", namecount);
                     exit(-1);
                    }
                    return(NAME);
                   }

<COMMENT_STARTED>{DSPLIT} {                                  /* Description separator found. Set context */
                    Cno=Cno+Getcommleng;                     /* to in description.                       */
                    BEGIN(IN_DESCRIP);                       /* ACTION:              */
                    DP(fprintf(stdout,"<DESCRIP_DELIM>\n");)    /*  Return DESCRIP_DELIM token */
                    return(DESCRIP_DELIM);
                   }

<COMMENT_STARTED>{COMMA} {                                   /* Match commas specially as these are used */
                     Cno=Cno+Getcommleng;                    /* to find extended lists of variable names */
                     DP(fprintf(stdout,"<COMMA>\n");)           /* ACTION:             */
                     return(COMMA);                          /*  Return COMMA token */
                     }

<COMMENT_STARTED>\n {
                    Lno=Lno+1;
                    Cno=1;
                    BEGIN(NOT_A_COMMENT);                     /* ACTION:           */
                    DP(fprintf(stdout,"\n<\\COMMENT_START>\n");) /*  Return EOL token */
                    return(EOL);
                    }

<COMMENT_STARTED>[ ] {
                                                              /* ACTION:             */
                    Cno=1;                                    /*  Return SPACE token */
                    return(SPACE);
                    }

<COMMENT_STARTED>.  {
                                                              /* ACTION:             */
                    Cno=1;                                    /*  Return PUNCT token */
                    return(PUNCT);
                    }

<IN_DESCRIP>.  {
               Cno=Cno+Getcommleng; 
               DP(fprintf(stdout,"<DESCRIP_TEXT>\n");)           /* ACTION:                          */
               DP(fprintf(stdout,"%s",Getcommtext);)             /*  Return DESCRIPT token and value */
               DP(fprintf(stdout,"\n<\\DESCRIP_TEXT>\n");)
               /* strdup text onto stack; increment text stack counter */
               descriparr[descriptcount]=strdup(Getcommtext);
               ++descriptcount; if ( descriptcount == MAX_DESCRIPARR ) {
                fflush(stdout);
                fprintf(stderr,"descriptcount == %d, need to increase MAX_DESCRIPARR in Getcomm.lex\n", descriptcount);
                exit(-1);
               }
               return(PUNCT);
               }

<IN_DESCRIP>\n  {
                Lno=Lno+1;
                Cno=Cno+Getcommleng;
                BEGIN(NOT_A_COMMENT);
                DP(fprintf(stdout,"<EOL>\n");)                   /* ACTION:           */
                DP(fprintf(stdout,"<\\COMMENT_START>\n");)       /*  Return EOL token */
                return(EOL);
                }

<NOT_A_COMMENT>\n      {                                    
                       Lno=Lno+1; Cno=1;                      /* ACTION:           */
                                                              /*  Skip             */
                       }

<NOT_A_COMMENT>.       {                                      
                       Cno=Cno+Getcommleng;                   /* ACTION:           */
                       }                                      /*  Skip             */

.                     {
                       Cno=Cno+Getcommleng;                   /* ACTION:           */
                      }                                       /*  Skip             */

\n                  {
                       Lno=Lno+1; Cno=1;                      /* ACTION:           */
                      }                                       /*  Skip             */
%%

int Getcommdebug;
main()
{

 Getcommparse();

}
