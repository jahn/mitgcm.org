/*

  Library of routines for recording information on
  F77/F90 code.

*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "TOKTAB.h"
#include "GLOBALS.h"
#include "DD.h"

/* We build a list of symbol names on each physcial line of text */
/* This list has a max. size of 200, but can be increased here.  */
#define MAX_NO_NAMES_PER_LINE 200
static char *nameBuffer[MAX_NO_NAMES_PER_LINE];
static int  lNameBuffer = 0;

F90symerror(s)
char *s;
{
  printf("Error: %s. Line no. %d\n",s,Lno);
}

/* Record a procedure start */
noteProcDef( name, lnumber, fname)
char *name; int  lnumber; char *fname;
{
   symTab *theSym, *prevEntry; char *upperCaseName;
   char *tmpNam;
   int i; int l;

   /* Free up the external list                                     */
   /* Think this is done because we have just entered a new routine */
   /* so we start afresh with the external list.                    */
   theSym=extTabHead;
   while ( theSym != NULL ) {
    free(theSym->symName); free(theSym->ucName);
    prevEntry=theSym;
    theSym=theSym->next;
    free(prevEntry);
   }
   extTabHead = NULL;

   /* Add name to procedure list */
   upperCaseName = strdup(name); l=strlen(upperCaseName);
   for (i=0;i<l;++i) { upperCaseName[i]=toupper(upperCaseName[i]); }

   printf("%s %s PROCEDURE <%s,%d>\n", upperCaseName, name, fname, lnumber );

   theSym = procTabHead; prevEntry = procTabHead;
   while ( theSym != NULL ) {
     if ( strcmp(theSym->ucName,upperCaseName) == 0 ) break;
     prevEntry = theSym;
     theSym = theSym->next;
   }
   if ( theSym == NULL ) {
    /* printf("No symbol table entry for %s\n",name); */
    ++procCount;
    theSym = (symTab *)malloc(sizeof(symTab));
    if ( theSym != NULL ) {
     theSym->symName  = strdup(name);
     theSym->ucName   = upperCaseName;
     theSym->useCount = 0;
     theSym->next     = NULL;
    }
    if ( prevEntry != NULL ) {
     prevEntry->next = theSym;
    } else {
     procTabHead = theSym;
    }
   } else {
    ++theSym->useCount;
    /* printf("Symbol %s used %d\n",name,theSym->useCount); */
   }
   currentProcedure = theSym->ucName;
   currentProcedureLine0 = lnumber;

   /* Now record name in general symbols list */
   tmpNam = strdup(currentProcedure);
   assertNameIsProcName();
   noteVariable(tmpNam, lnumber, fname);
   clearNameIsProcName();


   return;
}

/* Record a procedure call */
noteProcCall( name, lnumber, fname)
char *name; int  lnumber; char *fname;
{
   symTab *theSym, *prevEntry; char *upperCaseName;
   char *tmpNam;
   int i; int l;

   upperCaseName = strdup(name); l=strlen(upperCaseName);
   for (i=0;i<l;++i) { upperCaseName[i]=toupper(upperCaseName[i]); }

   printf("%s %s CALL <%s,%6.6d> \"%s\"\n", upperCaseName, name, fname, 
          lnumber, currentLineText+1);

   theSym = procTabHead; prevEntry = procTabHead;
   while ( theSym != NULL ) {
     if ( strcmp(theSym->ucName,upperCaseName) == 0 ) break;
     prevEntry = theSym;
     theSym = theSym->next;
   }
   if ( theSym == NULL ) {
    /* printf("No symbol table entry for %s\n",name); */
    ++procCount;
    theSym = (symTab *)malloc(sizeof(symTab));
    if ( theSym != NULL ) {
     theSym->symName  = strdup(name);
     theSym->ucName   = upperCaseName;
     theSym->useCount = 1;
     theSym->next     = NULL;
    }
    if ( prevEntry != NULL ) {
     prevEntry->next = theSym;
    } else {
     procTabHead = theSym;
    }
   } else {
    ++theSym->useCount;
    /* printf("Symbol %s used %d\n",name,theSym->useCount); */
   }

   /* Now record name in general symbols list */
   tmpNam = strdup(upperCaseName);
   noteVariable(tmpNam, lnumber, fname);

   return;
}

/* Record an external definition              */
/* Add it both to the procedure table and the */
/* active externals table.                    */
noteExtDef( name, lnumber, fname)
char *name; int  lnumber; char *fname;
{
   symTab *theSym, *prevEntry; char *upperCaseName;
   int i; int l;

   upperCaseName = strdup(name); l=strlen(upperCaseName);
   for (i=0;i<l;++i) { upperCaseName[i]=toupper(upperCaseName[i]); }

   printf("%s %s EXTERNAL <%s,%d>\n", upperCaseName, name, fname, lnumber );

   theSym = procTabHead; prevEntry = procTabHead;
   while ( theSym != NULL ) {
     if ( strcmp(theSym->ucName,upperCaseName) == 0 ) break;
     prevEntry = theSym;
     theSym = theSym->next;
   }
   if ( theSym == NULL ) {
    /* printf("No symbol table entry for %s\n",name); */
    ++procCount;
    theSym = (symTab *)malloc(sizeof(symTab));
    if ( theSym != NULL ) {
     theSym->symName  = strdup(name);
     theSym->ucName   = upperCaseName;
     theSym->useCount = 0;
     theSym->next     = NULL;
    }
    if ( prevEntry != NULL ) {
     prevEntry->next = theSym;
    } else {
     procTabHead = theSym;
    }
   } else {
    /* printf("Symbol %s used %d\n",name,theSym->useCount); */
   }

   theSym = extTabHead; prevEntry = extTabHead;
   while ( theSym != NULL ) {
     if ( strcmp(theSym->ucName,upperCaseName) == 0 ) break;
     prevEntry = theSym;
     theSym = theSym->next;
   }
   if ( theSym == NULL ) {
    /* printf("No symbol table entry for %s\n",name); */
    theSym = (symTab *)malloc(sizeof(symTab));
    if ( theSym != NULL ) {
     theSym->symName  = strdup(name);
     theSym->ucName   = strdup(upperCaseName);  /* dup because we free this later */
     theSym->useCount = 0;
     theSym->next     = NULL;
    }
    if ( prevEntry != NULL ) {
     prevEntry->next = theSym;
    } else {
     extTabHead = theSym;
    }
   } else {
    /* printf("Symbol %s used %d\n",name,theSym->useCount); */
   }
   return;
}

/* Record a variable name */
noteVariable(name, lnumber, fname)
const char *name; int lnumber; char *fname;
{
   char *upperCaseName;
   int i; int l;
   ddRecord *searchResult; ddRecord rec;

   /* int lStr;
   lStr = strlen(name);
   upperCaseName = (char *)malloc( lStr );
   upperCaseName = strncpy( upperCaseName, name, lStr); */

   upperCaseName = strdup(name);

   rec.name = upperCaseName;
   rec.hrefEntry      = NULL;
   rec.textEntry      = NULL;
   rec.unitsEntry     = NULL;
   rec.footNotesEntry = NULL;
   rec.active         = 1;
   
   searchResult = ddFind(&rec);

  if ( searchResult == NULL ) {
    searchResult = ddAdd(&rec);
    if ( inNameList == 1 ) { ddSetIsInNamelist(searchResult); }
    if ( inIfdef == 1 )    { ddSetIsInIfdef(searchResult);    }
    if ( nameIsProcName == 1 ) { ddSetIsProcName(searchResult);}
  } else {
    ++(searchResult->active);
    if ( inNameList == 1 ) { ddSetIsInNamelist(searchResult);  }
    if ( inIfdef == 1 )    { ddSetIsInIfdef(searchResult);     }
    if ( nameIsProcName == 1 ) { ddSetIsProcName(searchResult);}
  }
  
  /* Write to HTML buffer with appropriate HREF */
  strcat(currentLineHtml,"<A HREF=\"../");
  strcat(currentLineHtml,VARSUF);
  strcat(currentLineHtml,"/");
  strcat(currentLineHtml,searchResult->key);
  strcat(currentLineHtml,HTMLSUF);
  strcat(currentLineHtml,"\">");
  strcat(currentLineHtml,name);
  strcat(currentLineHtml,"</A>");
  /* Write table entry for this occurence of the variable */
  nameBuffer[lNameBuffer] = name;
  ++lNameBuffer;
  if ( lNameBuffer == MAX_NO_NAMES_PER_LINE ) {
   fprintf(stderr,"More than %d names on a single line. Increase MAX_NO_NAMES_PER_LINE.\n",
    MAX_NO_NAMES_PER_LINE);
   exit(0);
  }

}

void F90db_Newline()
{
  int i;
  /* Write out HTML buffer */
  fprintf (srcfd,"%s\n",currentLineHtml);

  /* Write variables table records */
  for (i=0;i<lNameBuffer;++i ){
   fprintf(tmpfd,"%s",nameBuffer[i]);
   fprintf(tmpfd,",<A HREF=../code/%s#%d_L>%d</A>",sHtmlName,Lno,Lno);
   fprintf(tmpfd,",<A HREF=../code/%s>%s</A>",sHtmlName,currentFile);
   /* fprintf(tmpfd,",<A HREF=%s>%s</A>",currentProcedure,currentProcedure); */
   fprintf(tmpfd,",<A HREF=../code/%s#%d_L>%s</A>",sHtmlName,
     currentProcedureLine0,currentProcedure);
   fprintf(tmpfd,",%s",currentLineHtml);
   /*printf("%s,%8.8d,%s,%s,%s\n", nameBuffer[i],
    Lno, currentFile, currentProcedure, currentLineHtml);*/
   fprintf(tmpfd,"\n");
  }

  /* Free name storage */
  for (i=0;i<lNameBuffer;++i ){
   free(nameBuffer[i]);
  }
  lNameBuffer=0;
  /* Clear HTML buffer */
  currentLineHtml[0] = (char)NULL;
}

/* Note when we are in/not in certain states */
/* NAMELIST */
void noteInNameList(name, lnumber, fname)
char *name; int  lnumber; char *fname;
{ char *tmpNam;
  inNameList = 1; 
  tmpNam = strdup(name);
  noteVariable(tmpNam, lnumber, fname);
}

/* #ifdef   */
void noteInIfdef()
{ inIfdef    = 1; }

/* Flag for saying name is/is not procedure name */
assertNameIsProcName()
{ nameIsProcName = 1; }
clearNameIsProcName()
{ nameIsProcName = 0; }

void newStatementNotify()
{ 
  inNameList = 0; 
  inIfdef    = 0;
}

void newLineNotify()
{ 
  inIfdef    = 0;
}
