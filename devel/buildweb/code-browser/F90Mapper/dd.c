/* $Id: dd.c,v 1.2 2003/07/23 17:54:35 edhill Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GLOBALS.h"
#include "DD.h"

/*====================================================================
   Package of routines for managing a table of names and associated 
   parameters. Used as tool for manipulating the data dictionary
   associated with a list of program variables.
   Data dictionary contains symbol names, certain symbol attributes
   and pointers to a definition for that name.
======================================================================*/

#define  ddBLOCK 100    /* Dictionary. Initial size ddBLOCK. Grown in */
ddRecord      *dd=NULL; /* Initial dictionary pointer.                */
ddRecord   *ddTmp=NULL;   
int   ddSize       = 0; /* Size of table.                             */
int   ddNused      = 0; /* No. slots used in table.                   */
int   ddCurrent    = 0; /* Current record.                            */
int   ddKey        = 0; /* Identifier key.                            */

char *ddkey();

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *ddAdd( rec )
    /* Add record at end of dictionary */
ddRecord *rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
  /* On first call create one block dd  */
  /* use malloc so we can realloc later */
  if ( dd == NULL ) {
   dd = (ddRecord *)malloc(sizeof(ddRecord)*ddBLOCK);
   if ( dd == NULL ) return((ddRecord *)NULL);
   ddSize = ddBLOCK;
  }

  /* No name so we can't insert */
  if ( rec->name == NULL ) return ((ddRecord *)NULL);

  /* Allocate more storage - if it is needed. */
  if ( ddCurrent == ddSize ) {
    /* Allocate another block of dd space.    */
    ddTmp=dd;
    dd = (ddRecord *)realloc(dd,sizeof(*dd)*ddBLOCK+sizeof(*dd)*ddSize);
    if ( dd == NULL ) {
     dd=ddTmp;
     return((ddRecord *)NULL);
    }
    ddSize = ddSize+ddBLOCK;
  }

  /* Now set values that are not NULL */
  dd[ddCurrent].name = strdup(rec->name);

  /* CNH Debug starts */
  printf("ddAdd name=\"%s\", rec. no. = %d\n",rec->name,ddKey+1);

  /* CNH Debug ends   */


  if ( dd[ddCurrent].name == NULL ) return((ddRecord *)NULL);
  dd[ddCurrent].id   = ddKey+1;
  dd[ddCurrent].key  = strdup(ddkey(ddKey+1));
  if ( rec->hrefEntry != NULL ){
    dd[ddCurrent].hrefEntry = strdup(rec->hrefEntry);
    if ( dd[ddCurrent].hrefEntry == NULL ) return((ddRecord *)NULL);
  } else {
   dd[ddCurrent].hrefEntry = NULL;
  }

  if ( rec->textEntry != NULL ){
    dd[ddCurrent].textEntry = strdup(rec->textEntry);
    if ( dd[ddCurrent].textEntry == NULL ) return((ddRecord *)NULL);
  } else {
    dd[ddCurrent].textEntry = NULL;
  }

  if ( rec->footNotesEntry != NULL ){
    dd[ddCurrent].footNotesEntry = strdup(rec->footNotesEntry);
    if ( dd[ddCurrent].footNotesEntry == NULL ) return((ddRecord *)NULL);
  } else {
    dd[ddCurrent].footNotesEntry = NULL;
  }

  if ( rec->unitsEntry != NULL ){
    dd[ddCurrent].unitsEntry = strdup(rec->unitsEntry);
    if ( dd[ddCurrent].unitsEntry == NULL ) return((ddRecord *)NULL);
  } else {
    dd[ddCurrent].unitsEntry = NULL;
  }

  dd[ddCurrent].active = rec->active;

  /* Set to not a namelist member by default */
  dd[ddCurrent].isInNameList = 0;
  /* Set to not an ifdef entry by default */
  dd[ddCurrent].isInIfdef    = 0;
  /* Set to not a procedure name by default */
  dd[ddCurrent].isProcName   = 0;

  /* Move current record pointer forward */
  if ( ddCurrent == ddNused ){
    ++ddNused;
  }
  ++ddCurrent; ++ddKey;
  return(dd+(ddCurrent-1));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int ddCompar( a, b )
           /* DD entry comparison routine. Used by ddSort */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *a;
ddRecord *b;
{
  int rc;
  rc = strcasecmp(a->name,b->name);
  return(rc);
}

int ddSort()
    /* Sort the DD table */
{
    int ddElSize;
    /* Sort the table */
    ddElSize = sizeof(*dd);
    qsort(dd,ddNused,ddElSize,ddCompar);
    ddCurrent=0;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int ddSetCurrent(n)
    /* Set current DD record */
int n;
{
 ddCurrent = n;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int ddGetCurrent(rec)
    /* Return current DD record */
ddRecord **rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
  /* Return if at or past end of table */
  if ( ddCurrent == ddNused   ) {
   return(0);
  }

  *rec = &dd[ddCurrent];
  ++ddCurrent;
  return(dd[ddCurrent-1].id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *ddFind(rec)
    /* Return current DD record */
    /* Usage:
       Routine can be called with ddRecord or with NULL. If record "rec"
       is not NULL search starts from top. If record is NULL search
       starts from where previous search finished. 
    */
ddRecord *rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
 static ddRecord *curRec = NULL;
 static int      curInd  = 0;

 /* Conditions under which we know nothing can be found. */
 if ( curRec == NULL && rec == NULL      ) return((ddRecord *)NULL);
 if ( rec != NULL && rec->name == NULL   ) return((ddRecord *)NULL);
 if ( rec == NULL && curInd >= ddNused-1 ) return((ddRecord *)NULL);

 /* It is worth looking */
 /* Start new search */
 if ( rec != NULL ) { curInd = 0; curRec = rec; }
 /* Do search */
 while ( curInd < ddNused ) {
  if ( strcmp(curRec->name,(dd[curInd]).name) == 0 ) {
   return(&(dd[curInd]));
  }
  ++curInd;
 }
 /* Nothing found */
 return((ddRecord *)NULL);
}
char *ddkey(n)
int n;
{
    return(base36(n));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void ddPrint()
    /* Prints DD table to standard out */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
 int      curInd  = 0;

 curInd = 0;
 /* Do print */
 while ( curInd < ddNused ) {
  printf("DD Record No. %d == \"%s\"", curInd, (dd[curInd]).name);
  printf(", NL = %d",     (dd[curInd]).isInNameList);
  printf(", IFDEF = %d",  (dd[curInd]).isInIfdef);
  printf(", PROC = %d",   (dd[curInd]).isProcName);
  printf("\n");
  ++curInd;
 }
 /* Nothing found */
 return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *ddSetIsInNamelist( rec )
  /* Tags dd entry used in NAMELIST */
ddRecord *rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
  rec->isInNameList=1;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *ddSetIsInIfdef( rec )
  /* Tags dd entry used in ifdef */
ddRecord *rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
  rec->isInIfdef=1;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ddRecord *ddSetIsProcName( rec )
  /* Tags dd entry used in ifdef */
ddRecord *rec;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
{
  rec->isProcName=1;
}
