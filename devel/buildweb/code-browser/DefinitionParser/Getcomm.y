/* 
 BISON Parser to recognise name definition records.
 A name definiton record ( or def ) has form

 variable(s) :: decription text

 It is only recognised when in the comment section of a 
 code. All lines of the form above are treat as def 
 records. All other comment or executable statements are 
 ignored.
 
*/
%{
#include "stdio.h"
#include "string.h"
int Lno; int Cno;
int i;
#include "GLOBALS.h"
%}


%union {
   int      LineNo;
   char    *symbolName;
}

%token <LineNo> COMMENT_START  
%token <LineNo> DESCRIP_DELIM
%token <LineNo> COMMA
%token <LineNo> SPACE
%token <LineNo> PUNCT
%token <LineNo> EOL
%token <symbolName> NAME
%token <symbolName> DESCRIP_TEXT

%%

input:  /* empty */
      | input comment_statement
;

/* 
   Definition statements 
*/
comment_statement:  def_with_terminal_names
                  | def_with_to_be_continued_names
                  | def_with_no_names
                  | other_comment
;

def_with_terminal_names: COMMENT_START  general_text descript { /*          Cases that are possible : Actions
                                                                 1. Names open   && descrip open  : add names, add descrip, close names
                                                                 2. Names closed && descrip open  : close descrip, close previous, 
                                                                                                    log previous, start new, open descrip,
                                                                                                    add descrip, open names, add names, 
                                                                                                    close names.
                                                                 3. Names closed && descrip closed: start new, open descrip,
                                                                                                    add descrip, open names, add names,
                                                                                                    close names.
                                                                */ 
                                                                /* fprintf(stdout,"def_with_terminal_names: line %d\n",Lno); */
                                                                /* Get the current line names            */
                                                                /* Get the current line description text */
                                                                def_with_terminal_names();
                                                                /* Free names and descript */
                                                                for(i=namecount-1;i>=0;--i) {
                                                                 free(namearr[i]);
                                                                }
                                                                namecount=0;
                                                                for(i=descriptcount-1;i>=0;--i) {
                                                                 free(descriparr[i]);
                                                                }
                                                                descriptcount=0;
                                                              }
;

def_with_to_be_continued_names: COMMENT_START general_text COMMA descript { 
                                                                /*
                                                                          Cases that are possible : Actions
                                                                 1. Names open   && descrip open  : add names, add descrip
                                                                 2. Names closed && descrip open  : close descrip, close previous,
                                                                                                    log previous, start new, open descrip,
                                                                                                    add descrip, open names, add names.
                                                                 3. Names closed && descrip closed: start new, open descrip,
                                                                                                    add descrip, open names, add names.
                                                                */
                                                                /* fprintf(stdout,"def_with_to_be_continued_names: line %d\n",Lno); */
                                                                /* Get the current line names            */
                                                                /* Get the current line description text */
                                                                def_with_to_be_continued_names();
                                                                /* Free names and descript */
                                                                for(i=namecount-1;i>=0;--i) {
                                                                 free(namearr[i]);
                                                                }
                                                                namecount=0;
                                                                for(i=descriptcount-1;i>=0;--i) {
                                                                 free(descriparr[i]);
                                                                }
                                                                descriptcount=0;
                                                               }
;

def_with_no_names: COMMENT_START   punct_only_text descript {/*
                                                                          Cases that are possible : Actions
                                                                 1. Names open   && descrip open  : add descrip
                                                                 2. Names closed && descrip open  : add descrip
                                                                 3. Names closed && descrip closed: syntax error
                                                             */
                                                             /* fprintf(stdout,"def_with_no_names: line %d\n",Lno); */
                                                             /* Get the current line description text */
                                                             def_with_no_names();
                                                             /* Free names and descript */
                                                             for(i=namecount-1;i>=0;--i) {
                                                              free(namearr[i]);
                                                             }
                                                             namecount=0;
                                                             for(i=descriptcount-1;i>=0;--i) {
                                                              free(descriparr[i]);
                                                             }
                                                             descriptcount=0;
                                                            }
                  | COMMENT_START descript                  {/* fprintf(stdout,"def_with_no_names: line %d\n",Lno); */
                                                             /* Get the current line description text */
                                                             def_with_no_names();
                                                             /* Free names and descript */
                                                             for(i=namecount-1;i>=0;--i) {
                                                              free(namearr[i]);
                                                             }
                                                             namecount=0;
                                                             for(i=descriptcount-1;i>=0;--i) {
                                                              free(descriparr[i]);
                                                             }
                                                             descriptcount=0;
                                                            }
;

other_comment: COMMENT_START general_text EOL { /* Free names and descript */
                                                for(i=namecount-1;i>=0;--i) {
                                                 free(namearr[i]);
                                                }
                                                namecount=0;
                                                for(i=descriptcount-1;i>=0;--i) {
                                                 free(descriparr[i]);
                                                }
                                                descriptcount=0;
                                              }
             | COMMENT_START EOL              { /* Free names and descript */
                                                for(i=namecount-1;i>=0;--i) {
                                                 free(namearr[i]);
                                                }
                                                namecount=0;
                                                for(i=descriptcount-1;i>=0;--i) {
                                                 free(descriparr[i]);
                                                }
                                                descriptcount=0;
                                              }
;

/*
   One record of the name_list block
*/
general_text:  general_text general_text_element
             | general_text_element                    {}
;

general_text_element:  NAME                            {}
                     | punct_only_text                 {}
;

punct_only_text : punct_only_text punct_text_element
                | punct_text_element                   {}
;

punct_text_element: COMMA                              {}
                  | non_comma_punct_text               {}
;

non_comma_punct_text: space_list                       {}
                    | PUNCT                            {}
;

space_list:       space_list space_element
                | space_element
;

space_element: SPACE {}
;

descript: DESCRIP_DELIM general_text EOL { /* Get the description text */ }
        | DESCRIP_DELIM EOL              {}
;

%%

/* Buffers for storing name blocks and definition blocks */
#define MAX_NLIST 1000
char *nlist[MAX_NLIST];
int  nlc=0;

#define MAX_DLIST 10000
char *dlist[MAX_DLIST];
int  dlc=0;

Getcommerror(s)
char *s;
{
  printf("Error: %s. Line no. %d\n",s,Lno);
}

def_with_terminal_names()
/* 
  == Take action when a "def_with_terminal_names" pattern is recognised ==

            Cases that are possible  : Actions
     ================================:=====================================
   1. Names open   && descrip open   : add names, add descrip, close names
      This is the last record of a   :
      name block. It will have been  :
      preceeded by one or more name  :
      block records with continue    :
      markers.                       :
                                     :
   2. Names closed && descrip open   : close descrip, close previous,
      This is the start of a new     : log previous, start new, open descrip,
      name block. This name block    : add descrip, open names, add names,
      only has one name record.      : close names.
      Need to write out the          :
      preceeding name and descript   :
      blocks.                        :
                                     :
   3. Names closed && descrip closed : start new, open descrip,
      This is right at start of      : add descrip, open names, add names,
      execution and this is the      : close names.
      first name block record.       :
*/
{
 int i,j,k;/* Loop counters          */
 int sl;   /* String length          */

 if ( name_open == 1 && descript_open == 1 ) {
  /* Append to existing block and close names for that block */
  for (i=0;i<namecount;++i) {
   nlist[i+nlc] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i+dlc] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
  /* Close names */
  name_open = 0;
 }

 if ( name_open == 0 && descript_open == 1 ) {
  /* Flush previous name block and descrip block and start new blocks. */
  /* New name block is closed after this name record is added.         */
  for (j=0;j<nlc;++j) { 
   fprintf(stdout,"%-50s",nlist[j]);
   fprintf(stdout," {TEXT=\"");
   for (i=0;i<dlc;++i) {
    /* Substitue " with a space for now because VarDic parser gets confused */
    sl=strlen(dlist[i]);
    for (k=0;k<sl;++k) {
     if ( dlist[i][k] == '"' ) {
      dlist[i][k] = ' ';
     }
    }
    fprintf(stdout,"%s",dlist[i]);
   }
   fprintf(stdout,"\";}\n");
  }

  /* Free up block strings */
  for (i=nlc-1;i>=0;--i){ free(nlist[i]); } ; nlc = 0;
  for (i=dlc-1;i>=0;--i){ free(dlist[i]); } ; dlc = 0;

  /* Start new name block and descrip block */
  name_open=0; descript_open=1;
  nlc=0; dlc=0;
  for (i=0;i<namecount;++i) {
   nlist[i] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }

 if ( name_open == 0 && descript_open == 0 ) {
  /* Start new name block and descrip block.                   */
  /* New name block is closed after this name record is added. */
  name_open=0; descript_open=1;
  nlc=0; dlc=0;
  for (i=0;i<namecount;++i) {
   nlist[i] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }
}
def_with_to_be_continued_names() 
/*
            Cases that are possible : Actions
   =================================:======================================
   1. Names open   && descrip open  : add names, add descrip
      This is a continuation of an  : leave names open.
      already open name and         :
      description block.            :
   2. Names closed && descrip open  : close descrip, close previous,
      This is the start of a new    : log previous, start new, open descrip,
      name and description block.   : add descrip, open names, add names.
                                    : leave names open.
                                    :
   3. Names closed && descrip closed: start new, open descrip,
      Beginning of parsing. First   : add descrip, open names, add names.
      block encountered has cont.   : leave names open.
      style name record.            :
*/
{
 int i,j,k;/* Loop counters          */
 int sl;   /* String length          */


 if ( name_open == 1 && descript_open == 1 ) {
  /* Append to existing block. Leave block open */
  for (i=0;i<namecount;++i) {
   nlist[i+nlc] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i+dlc] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }

 if ( name_open == 0 && descript_open == 1 ) {
  /* Flush previous name block and descrip block and start new blocks. */
  /* New name block left open.                                         */
  for (j=0;j<nlc;++j) { 
   fprintf(stdout,"%-50s",nlist[j]);
   fprintf(stdout," {TEXT=\"");
   for (i=0;i<dlc;++i) {
    /* Substitue " with a space for now because VarDic parser gets confused */
    sl=strlen(dlist[i]);
    for (k=0;k<sl;++k) {
     if ( dlist[i][k] == '"' ) {
      dlist[i][k] = ' ';
     }
    }
    fprintf(stdout,"%s",dlist[i]);
   }
   fprintf(stdout,"\";}\n");
  }

  /* Free up block strings */
  for (i=nlc-1;i>=0;--i){ free(nlist[i]); } ; nlc = 0;
  for (i=dlc-1;i>=0;--i){ free(dlist[i]); } ; dlc = 0;

  /* Start new name block and descrip block */
  /* Leave both open.                       */
  name_open=1; descript_open=1;
  nlc=0; dlc=0;
  for (i=0;i<namecount;++i) {
   nlist[i] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }

 if ( name_open == 0 && descript_open == 0 ) {
  /* Start new name block and descrip block.                   */
  /* Leave both open.                                          */
  name_open=1; descript_open=1;
  nlc=0; dlc=0;
  for (i=0;i<namecount;++i) {
   nlist[i] = strdup(namearr[i]);
  }
  nlc=nlc+namecount;
  for (i=0;i<descriptcount;++i) {
   dlist[i] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }

}

def_with_no_names() 
/*
            Cases that are possible : Actions
      ==============================:==============
   1. Names open   && descrip open  : add descrip
                                    :
   2. Names closed && descrip open  : add descrip
                                    :
   3. Names closed && descrip closed: syntax error

*/
{
 if ( descript_open == 1 ) {
  /* Append to existing block */
  for (i=0;i<descriptcount;++i) {
   dlist[i+dlc] = strdup(descriparr[i]);
  }
  dlc=dlc+descriptcount;
 }

 if ( descript_open == 0 ) {
  /* Description but no names */
  /* Ignore with warning      */
  fprintf(stderr,"WARNING: Line no. %d looks like a description, but no name block is active.\n",Lno-1);
 }
 
}

