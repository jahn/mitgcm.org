/* $Id: $ */

/*
 File-directory table routines
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "DD.h"
#include "FD.h"
#include "GLOBALS.h"

/*
 Set initial state of file-directory table
*/
fdInit()
{
 fdList = NULL;
 fdListHead = fdList;
 fdListTail = fdList;
}

/*
 Store directory, file and associated html source info.
 Info. is stored in table of form
   dirname1 file1 html
            file2 html
            file3 html
              :

   dirname2 filen html
            filem html
            fileo html
              :
  etc..,,
*/
fdAdd( srcDirName, srcFileName, sHtmlName )
char *srcDirName;
char *srcFileName;
char *sHtmlName;
{
 int nComp, nCompPrev;
 int nFd;
 /* For search file names */
 fdSTab *sfList, *sfListBefore, *sfListAfter;
 fdDTab *fdListAfter, *fdListBefore;

 fdList = fdListHead;
 fdListBefore = NULL;
 fdListAfter  = NULL;
 nFd    = 0;

 /* Search for the directory name                   */
 /* Insert new names in alphabetical order          */
 nComp = -1;
 while ( fdList != NULL ) {
  /* nComp +ve arg1 alphabetically before arg2
     nComp 0   arg1 == arg2
     nComp -ve arg1 alphabetically after arg2
  */
  nComp = -strcmp( srcDirName, (fdList->dir).name );
  ++nFd;
  if ( nComp >= 0 ) {
   fdListAfter = fdList;
   break;
  }
  fdListBefore = fdList;
  fdList = fdList->next;
 }

 /* No match for directory name */
 if ( nComp != 0 ) {
  /* Create an entry */
  fdList = (fdDTab *) malloc(sizeof(fdDTab));
  fdList->next  = fdListAfter;
  fdList->prev  = fdListBefore;
  fdList->sList = NULL;
  (fdList->dir).name = strdup(srcDirName);
  /* Insert into list */
  if ( fdListHead == NULL ) {
   /* First entry */
   fdListHead = fdList; fdListTail = fdList;
  } 
  if ( fdListAfter == fdListHead ) {
   /* Adding at head */
   fdListHead = fdList;
  }
  /* Inserts between existing entries */
  if ( fdList->next != NULL ) fdList->next->prev = fdList;
  if ( fdList->prev != NULL ) fdList->prev->next = fdList;
 }

 /* Search through file names paired with directory */
 /* Insert new names in alphabetical order          */
 sfList = fdList->sList;
 sfListBefore = NULL;
 sfListAfter  = NULL;
 nFd       = 0;
 nComp     = -1;
 while ( sfList != NULL ) {
  /* nComp +ve arg1 alphabetically before arg2
     nComp 0   arg1 == arg2
     nComp -ve arg1 alphabetically after arg2
  */
  nComp = -strcmp( srcFileName, (sfList->fileNam).name );
  if ( nComp >= 0 ) {
   sfListAfter  = sfList;
   break;
  }
  ++nFd;
  sfListBefore = sfList;
  sfList = sfList->next;
 }
 /* No match for file name */
 if ( nComp != 0 ) {
  /* Create an entry */
  sfList = (fdSTab *) malloc(sizeof(fdSTab));
  sfList->fileNam.name  = strdup(srcFileName);
  sfList->fileNam.hname = strdup(sHtmlName);
  sfList->prev  = sfListBefore;
  sfList->next  = sfListAfter;
  if ( fdList->sList == NULL ) {
   /* First file for this directory */
   fdList->sList = sfList;
  }
  if ( fdList->sList == sfList->next ) {
   /* Added to head of list */
   fdList->sList = sfList;
  }
  if ( sfList->prev != NULL ) { sfList->prev->next = sfList; }
  if ( sfList->next != NULL ) { sfList->next->prev = sfList; }
 }

}

/*
 Print fd tables.
*/
fdPrint()
{
 int nComp;
 int nFd;
 fdSTab *sfList, *sfListTail;

 fdList = fdListHead;
 nFd    = 0;

 /* Search for the directory name */
 while ( fdList != NULL ) {
  printf("FD %s",fdList->dir.name);
  /* Search through file names paired with directory */
  sfList = fdList->sList;
  sfListTail = sfList;
  nFd    = 0;
  while ( sfList != NULL ) {
   printf(", %s<%s>",sfList->fileNam.name,sfList->fileNam.hname);
   sfList = sfList->next;
  }
  printf("\n");
  fdList = fdList->next;
 }

}

/*
 Write fd directories.
*/
fdDirList(FILE *o)
{
 int nComp;
 int nFd;
 fdSTab *sfList, *sfListTail;
 char subURL[MAXPATHNAM];

 fdList = fdListHead;
 nFd    = 0;

 /* List directory names to DFMenu */
 while ( fdList != NULL ) {
  ++nFd;
  sprintf(subURL,"%s/%s_dir%d%s target=codeBrowserWindow",
   rootDir,SFDICT,nFd,HTMLSUF);
  html_entryli(o,fdList->dir.name,subURL,"h4");
  fdList = fdList->next;
 }

}

/*
 Write fd entry by letter
*/
fdFlistAlpha(FILE *o)
{
 int nComp;
 int nFd;
 fdSTab *sfList, *sfListTail;
 char subURL[MAXPATHNAM];
 char let1[2];
 char lolet[27] = "abcdefghijklmnopqrstuvwxyz";
 char uplet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
 int i;
 int f1, f0;

 fdList = fdListHead;
 nFd    = 0;
 let1[0]=' ';
 let1[1]='\0';
 f0=0;

 /* Make links for alphabetic file tables */
 for (i=0;i<27;++i) {
  fdList = fdListHead;
  f1=0;
  while ( fdList != NULL ) {
   sfList = fdList->sList;
   while ( sfList != NULL ) {
    if ( ( sfList->fileNam.name[0] == lolet[i] ||
           sfList->fileNam.name[0] == uplet[i] ) &&
         f1 == 0 ) {
     if ( f0 == 0 ) { html_li(o); f0=1; html_hn(o,"h4"); }
     let1[0]=uplet[i];
     sprintf(subURL,"%s/%s_pref%c%s target=codeBrowserWindow",
      rootDir,SFDICT,uplet[i],HTMLSUF);
     html_entry(o,let1,subURL);fprintf(o,", ");
     f1=1;  
    }
    sfList = sfList->next;
   }
   fdList = fdList->next;
  }
 }
 if ( f0 == 1 ) { html_ehn(o,"h4"); html_eli(o); }
}

fdTab() 
{
FILE *allTabfd, *htout;
char allTabName[MAXPATHNAM];
 fdSTab *sfList, *sfListTail;
 int dNo;
 ddRecord rec, *searchResult;

 fdList = fdListHead;

 /* Single list */
 sprintf(allTabName,"%s/%s%s",rootDir,SFDICT,HTMLSUF);
 allTabfd=fopen(allTabName,"w");
 htout=allTabfd;

 html_start(htout);
  html_FlistTabStart(htout);
  html_FlistColHeader(htout);
  while ( fdList != NULL ) {
   sfList = fdList->sList;
   while ( sfList != NULL ) {
    rec.name = sfList->fileNam.name; rec.hrefEntry      = NULL;
    rec.textEntry      = NULL; rec.unitsEntry     = NULL;
    rec.footNotesEntry = NULL; rec.active         = 1;
    searchResult       = ddFind(&rec); 
    if ( searchResult != NULL ) {
     html_FlistColRecord(htout,
     fdList->dir.name,
     sfList->fileNam.name,
     sfList->fileNam.hname,
     searchResult->textEntry);
    } else {
     html_FlistColRecord(htout,
     fdList->dir.name,
     sfList->fileNam.name,
     sfList->fileNam.hname,
     " ");    /* NO DEFINITION */
    }
    sfList = sfList->next;
   }
   fdList = fdList->next;
  }
  html_FlistTabStop(htout);
 html_end(htout);
 fprintf(htout,"\n");         

 fclose(allTabfd);

 /* Directory at a time tables */
 fdList = fdListHead;
 dNo=0;
 while ( fdList != NULL ) {
  ++dNo;
  sprintf(allTabName,"%s/%s_dir%d%s",rootDir,SFDICT,dNo,HTMLSUF);
  allTabfd=fopen(allTabName,"w");
  htout=allTabfd;
  html_start(htout);
  html_FlistTabStart(htout);
  html_FlistColHeader(htout);
  sfList = fdList->sList;
  while ( sfList != NULL ) {
   rec.name = sfList->fileNam.name; rec.hrefEntry      = NULL;
   rec.textEntry      = NULL; rec.unitsEntry     = NULL;
   rec.footNotesEntry = NULL; rec.active         = 1;
   searchResult       = ddFind(&rec); 
   if ( searchResult != NULL ) {
    html_FlistColRecord(htout,
    fdList->dir.name,
    sfList->fileNam.name,
    sfList->fileNam.hname,
    searchResult->textEntry);
   } else {
    html_FlistColRecord(htout,
    fdList->dir.name,
    sfList->fileNam.name,
    sfList->fileNam.hname,
     " ");   /* NO DEFINITION */
   }
   sfList = sfList->next;
  }
  html_FlistTabStop(htout);
  html_end(htout);
  fprintf(htout,"\n");         
  fclose(allTabfd);
  fdList = fdList->next;
 }

 /* Alphabetic tables         */
 fdFlistAlphaTab();

}

/*
 Write fd entry by letter
*/
fdFlistAlphaTab()
{
 int nComp; int nFd;
 fdSTab *sfList, *sfListTail;
 char subURL[MAXPATHNAM];
 char let1[2];
 char lolet[27] = "abcdefghijklmnopqrstuvwxyz";
 char uplet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
 int i; int f1, f0;
 FILE *htout;
 ddRecord rec, *searchResult;

 fdList = fdListHead;
 nFd    = 0;
 let1[0]=' ';
 let1[1]='\0';
 f0=0;

 /* Make alphabetic file tables */
 for (i=0;i<27;++i) {
  fdList = fdListHead;
  f1=0;
  while ( fdList != NULL ) {
   sfList = fdList->sList;
   while ( sfList != NULL ) {
    if ( sfList->fileNam.name[0] == lolet[i] ||
         sfList->fileNam.name[0] == uplet[i] )
    {
     if ( f1 == 0 ) {
      sprintf(subURL,"%s/%s_pref%c%s",rootDir,SFDICT,uplet[i],HTMLSUF);
      htout=fopen(subURL,"w");
      f1=1;  
      html_start(htout); html_FlistTabStart(htout); html_FlistColHeader(htout);
     }
     rec.name = sfList->fileNam.name; rec.hrefEntry      = NULL;
     rec.textEntry      = NULL; rec.unitsEntry     = NULL;
     rec.footNotesEntry = NULL; rec.active         = 1;
     searchResult       = ddFind(&rec); 

     if ( searchResult != NULL ) {
      html_FlistColRecord(htout,
       fdList->dir.name,
       sfList->fileNam.name,
       sfList->fileNam.hname,
       searchResult->textEntry);
     } else {
      html_FlistColRecord(htout,
       fdList->dir.name,
       sfList->fileNam.name,
       sfList->fileNam.hname,
       " "); /*  NO DEFINITION */

     }
    }
    sfList = sfList->next;
   }
   fdList = fdList->next;
  }
  if ( f1 == 1 ) {
   html_FlistTabStop(htout); html_end(htout); fprintf(htout,"\n");         
   fclose(htout);
  }
 }
}
