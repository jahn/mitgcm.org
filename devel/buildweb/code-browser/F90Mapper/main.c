/* $Id: main.c,v 1.3 2004/02/17 15:37:34 edhill Exp $ */

/*
 Driver routine for generating hypertext instrumented code
 from a tree of Fortran source. Hypertext instrumentation allows
 symbols (subroutine names, variable names etc... )
 to be selected in a browser. When a symbol is selected 
 a table showing the definition of that symbol and all
 its occurences within the full code tree appears.
 For each occurence synopsis information, for example
 the line number the file name and the source text of the 
 line referred to is shown.
 This table is also hypertext. By selecting entries in the
 table, for example the line number or the file name or 
 a variable in the source text line that is reproduced
 other elements of the hypertext can be brought up.
 A mechanism for displaying attributes related to symbols
 is also includeed. Attributes describe what class or
 classes a symbol belongs to, for example is the symbol
 a function name, an input parameter, a compile time
 operator etc... Summary tables are also generated
 for viewing the output by attribute.
 A mechanism for linking to an external document, such
 as a text manual, is also included.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "DD.h"
#include "FD.h"
#include "GLOBALS.h"

/* Variable Dictionary Parser */
FILE *VarDicin;
int  VarDicParseError = 0;
int VarDicdebug;
char *vDictTitleWord = NULL;
static char vDictDefaultTitleWord[] = "DICTIONARY";

/* F90 Symbol Parser */
FILE *F90symin;
int  nSrcFile=0;  /* Counter for source file number */
int F90symdebug;

/* Command Line Arguments */
struct{
  char *dictFile;     /* Variable dictionary file */
  char *outDir;       /* Output directory         */
  int  nSFiles;       /* No. files in source list */
  char **srcFiles;    /* Files containing source  */
       } argList;

main (argc, argv)
int argc;
char *argv[];
{
 char vdictName[MAXPATHNAM];
 char procdictName[MAXPATHNAM];
 char parmdictName[MAXPATHNAM];
 char compdictName[MAXPATHNAM];
 FILE *htout;

 /* Process inout args */
 if ( GetArgs(argc, argv) == 0 ) {
  fprintf(stderr,
          "Usage: %s [-d Dictionary] [-o OutputDirectory] file(s)\n",argv[0]);
  exit(0);
 }

 /* Create output directory */
 if ( argList.outDir == NULL ) {
  argList.outDir = OUTDIR; 
 }
 if ( makeOutputDirectories(argList.outDir) != 0 ) {
  fprintf(stderr,"Unables to create output directory structure\n");
  fprintf(stderr,"%s\n",argList.outDir);
  fprintf(stderr,"%s/%s\n",argList.outDir,SRCSUF);
  fprintf(stderr,"%s/%s\n",argList.outDir,VARSUF);
  exit(0);
 }
 
 /* Scan input name definition table */
 VarDicdebug=0;
 ParseVarDic();
 if ( VarDicParseError != 0 )
  {
   if ( argList.dictFile != NULL ) {
    fprintf(stderr,"Error(s) occured reading dictionary \"%s\".",
     argList.dictFile);
   }
   fprintf(stderr," Program ending.\n");
   exit(0);
  }

 /* Scan code to get all variable names */
  /*  Set initial state of F77/F90 analyser */
  F90symdebug=0;
  inNameList=0;
  inIfdef=0;
  clearNameIsProcName();
  /* Run the analysis */
  ParseF90Code();

 /* Sort the table of variable uses */
 tblSort();

 /* Print out the data dictionary */
 ddPrint();

 /* Generate individual variable description tables */
 GenerateVarTables();

 /* Generate large single table */
 ddSort();

 sprintf(vdictName,"%s/%s",rootDir,VDICT);
 vdictfd = fopen(vdictName,"w");

 sprintf(procdictName,"%s/%s%s",rootDir,PROCDICT,HTMLSUF);
 procdictfd = fopen(procdictName,"w");

 sprintf(parmdictName,"%s/%s%s",rootDir,PARMDICT,HTMLSUF);
 parmdictfd = fopen(parmdictName,"w");

 sprintf(compdictName,"%s/%s%s",rootDir,COMPDICT,HTMLSUF);
 compdictfd = fopen(compdictName,"w");


 /* Table of all symbols */
 GenerateVDict();
 /* Table of just procedure names */
 ddSetCurrent(0);
 GenerateProcDict();
 /* Table of just compile time switching symbols */
 ddSetCurrent(0);
 GeneratePrecompDict();
 /* Table of just runtime parameters */
 ddSetCurrent(0);
 GenerateParamDict();

 /* Generate index of variables */
 ddSetCurrent(0);
 GenerateVDictIndex();

 ddSetCurrent(0);
 /* Generate tables of source files and directories */
 fdTab();

 /* Generate the HTML menus */
 /* Top level menu */
 mainMenu();
 /* Submenu for browsing all symbol list */
 ddSetCurrent(0);
 allSymbolsMenu();
 /* Submenu for browsing functions and procedures list */
 ddSetCurrent(0);
 allSubfuncMenu();
 /* Submenu for browsing runtime parameters list */
 ddSetCurrent(0);
 allRTParmMenu();
 /* Submenu for browsing compile time switches list */
 ddSetCurrent(0);
 allCTParmMenu();
 /* Submenu for browsing source by directory and file name */
 allDFMenu();

}

mainMenu() {
 FILE *htout;
 /* Try a bit of extra HTML */
 /* Main Menu */
 htout=fopen("code_reference.htm","w");
 html_start(htout); html_ul(htout);

 html_entryli(htout,"Overview","callTree.html target=mainBodyFrame","h3");
 html_entryli(htout,"Subroutines and Functions","code_reference-subfunc_exp.htm","h3");
 html_entryli(htout,"Runtime Parameters","code_reference-rtparm_exp.htm","h3");
 html_entryli(htout,"Compile Time Parameters","code_reference-ctparm_exp.htm","h3");
 html_entryli(htout,"Source Files","code_reference-sf_exp.htm","h3");
 html_entryli(htout,"All Symbols","code_reference-vi_exp.htm","h3");

 html_eul(htout); html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);

}

allSymbolsMenu() {
 FILE *htout;
 ddRecord *curRec;
 char curLetter = ' ';
 char clstr[2];
 char subURL[MAXPATHNAM];

 /* VI Sub-Menu */
 htout=fopen("code_reference-vi_exp.htm","w");
 html_start(htout); 
 fprintf(htout,"\n");
 html_ul(htout);
 fprintf(htout,"<H3>%s</H3>","Variable Index");

  html_ul(htout);
   html_entryli(htout,"All (compact form)","vdb/index.htm target=codeBrowserWindow","h4");
   html_entryli(htout,"All (detailed form)","vdb/vdict.htm target=codeBrowserWindow","h4");
   html_li(htout);
    while ( ddGetCurrent(&curRec) != 0 ){
     if ( toupper(curRec->name[0]) != curLetter ){ 
      curLetter = toupper(curRec->name[0]);
      sprintf(subURL,"%s/%s_pref%c%s target=codeBrowserWindow",
              rootDir,"vdict",curLetter,HTMLSUF);
      clstr[0]=curLetter;clstr[1]='\0';
      html_entry(htout,clstr,subURL,"h4"); fprintf(htout,", ");
     }
    }
   html_eli(htout);
  html_eul(htout);

 html_eul(htout); 
 html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);
}

allSubfuncMenu() {
 FILE *htout;
 ddRecord *curRec;
 char curLetter = ' ';
 char clstr[2];
 char subURL[MAXPATHNAM];

 /* VI Sub-Menu */
 htout=fopen("code_reference-subfunc_exp.htm","w");
 html_start(htout);
 fprintf(htout,"\n");

 fprintf(htout,"<H3>%s</H3","Subroutines and Functions");
  html_ul(htout);
   sprintf(subURL,"%s/%s%s target=codeBrowserWindow",
           rootDir,PROCDICT,HTMLSUF); 
   html_entryli(htout,"All",subURL,"h4");
   html_li(htout);
    while ( ddGetCurrent(&curRec) != 0 ){
     if ( curRec->isProcName == 1 ) {
      if ( toupper(curRec->name[0]) != curLetter ){ 
       curLetter = toupper(curRec->name[0]);
       sprintf(subURL,"%s/%s_pref%c%s target=codeBrowserWindow",
               rootDir,PROCDICT,curLetter,HTMLSUF);
       clstr[0]=curLetter;clstr[1]='\0';
       html_entry(htout,clstr,subURL,"h4"); fprintf(htout,", ");
      }
     }
    }
   html_eli(htout);
  html_eul(htout);

 html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);
}

allRTParmMenu() {
 FILE *htout;
 ddRecord *curRec;
 char curLetter = ' ';
 char clstr[2];
 char subURL[MAXPATHNAM];

 /* RTparm Sub-Menu */
 htout=fopen("code_reference-rtparm_exp.htm","w");
 html_start(htout);
 html_start(htout);

 fprintf(htout,"<H3>%s</H3>","Runtime Parameters");
 html_ul(htout);
   sprintf(subURL,"%s/%s%s target=codeBrowserWindow",
           rootDir,PARMDICT,HTMLSUF); 
   html_entryli(htout,"All",subURL,"h4");
   html_li(htout);
    while ( ddGetCurrent(&curRec) != 0 ){
     if ( curRec->isInNameList == 1 ) {
      if ( toupper(curRec->name[0]) != curLetter ){ 
       curLetter = toupper(curRec->name[0]);
       sprintf(subURL,"%s/%s_pref%c%s target=codeBrowserWindow",
               rootDir,PARMDICT,curLetter,HTMLSUF);
       clstr[0]=curLetter;clstr[1]='\0';
       html_entry(htout,clstr,subURL,"h4"); fprintf(htout,", ");
      }
     }
    }
   html_eli(htout);
  html_eul(htout);

 html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);
}

allCTParmMenu() {
 FILE *htout;
 ddRecord *curRec;
 char curLetter = ' ';
 char clstr[2];
 char subURL[MAXPATHNAM];

 /* CTparm Sub-Menu */
 htout=fopen("code_reference-ctparm_exp.htm","w");
 html_start(htout); html_ul(htout);
 fprintf(htout,"\n");

 html_entryli(htout,"Overview","callTree.html target=mainBodyFrame","h3");
 html_entryli(htout,"Subroutines and Functions","code_reference-subfunc_exp.htm","h3");
 html_entryli(htout,"Runtime Parameters","code_reference-rtparm_exp.htm","h3");
 html_entryli(htout,"Compile Time Parameters","code_reference.htm","h3");
  html_ul(htout);
   sprintf(subURL,"%s/%s%s target=mainBodyFrame",
           rootDir,COMPDICT,HTMLSUF); 
   html_entryli(htout,"All",subURL,"h4");
   html_li(htout);
    while ( ddGetCurrent(&curRec) != 0 ){
     if ( curRec->isInIfdef == 1 ) {
      if ( toupper(curRec->name[0]) != curLetter ){ 
       curLetter = toupper(curRec->name[0]);
       sprintf(subURL,"%s/%s_pref%c%s target=mainBodyFrame",
               rootDir,COMPDICT,curLetter,HTMLSUF);
       clstr[0]=curLetter;clstr[1]='\0';
       html_entry(htout,clstr,subURL,"h4"); fprintf(htout,", ");
      }
     }
    }
   html_eli(htout);
  html_eul(htout);
 html_entryli(htout,"Source Files","code_reference-sf_exp.htm","h3");
 html_entryli(htout,"All Symbols","code_reference-vi_exp.htm","h3");

 html_eul(htout); html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);
}

allDFMenu() {
 FILE *htout;
 ddRecord *curRec;
 char curLetter = ' ';
 char clstr[2];
 char subURL[MAXPATHNAM];

 /* CTparm Sub-Menu */
 htout=fopen("code_reference-sf_exp.htm","w");
 html_start(htout);
 fprintf(htout,"\n");
 fprintf(htout,"<H3>%s</H3>","Source Tree");

 html_ul(htout);
  sprintf(subURL,"%s/%s%s target=codeBrowserWindow",
          rootDir,SFDICT,HTMLSUF);
  html_entryli(htout,"All",subURL,"h4");
  fdFlistAlpha(htout);
  fdDirList(htout);
  html_eli(htout);
 html_eul(htout);

 html_end(htout);
 fprintf(htout,"\n");
 fclose(htout);
}

int GetArgs( argc, argv)
    /* Process inout args */
int argc; char *argv[];
{
  char *curArg; int i; char curOp; int rc;

  rc =1;

  for ( i = 1; i<argc; ++i){
   curArg = argv[i];
   if ( curArg[0] == '-' ) {
    curOp = argv[i][1];
   } else {
    switch (curOp) {
     case 'd':
      argList.dictFile = argv[i];
      curOp='\0';
      break;
     case 'o':
      curOp='\0';
      argList.outDir = argv[i];
      break;
     case '\0':
      if ( argList.srcFiles  == NULL ){
       argList.srcFiles = (char **)malloc( sizeof(*(argList.srcFiles)) );
      } else {
       argList.srcFiles = (char **)
        realloc(argList.srcFiles,
                (argList.nSFiles+1)*sizeof(*(argList.srcFiles)) );
      }
      argList.nSFiles = argList.nSFiles+1;
      if ( argList.srcFiles != NULL ) {
       *(argList.srcFiles+(argList.nSFiles-1)) = argv[i];
      }
      break;
     default:
      fprintf(stderr,"Unknown option \"%c\"\n",curOp);
      curOp='\0';
      rc = 0;
      break;
    }
   }
  }
  return(rc);
}

VarDicerror(s)
char *s;
{
  fprintf(stderr,"Error: unrecognised input. <%s> Line %d Column %d\n",
   currentFile,Lno,Cno);
  VarDicParseError=1;
}

ParseVarDic()
{
 int i; FILE *file;
 /* Parser variable initialisation */
 curName      = NULL;
 curHref      = NULL;
 curText      = NULL;
 curUnits     = NULL;
 curFootNotes = NULL;

 if ( argList.dictFile != NULL ){
  file = fopen(argList.dictFile,"r");
  if (!file){
   fprintf(stderr,"Unable to open file \"%s\"\n",argList.dictFile);
  } 
  else 
  {VarDicin = file; currentFile=argList.dictFile;
   Lno=1; Cno=1;
   VarDicparse();
   fclose(file);
  }
 } 
}

GenerateParamDict()
/*
 Generate tables of runtime parameters
*/
{
 ddRecord *curRec;
 char *note;
 char curLetter = ' ';
 FILE *vdictfd_sub = NULL;
 char vdictSubName[MAXPATHNAM];
 char vDictSubHeader[1024];
 static char vDictHeader[] = "Alphabetic table of All Runtime Parameters";

 fprintf(stderr,"Hi there \n");
 /* HTML headers */
 vDictTitleWord=vDictHeader;
 vDictStart(parmdictfd);
 vDictTitleWord=NULL;

 /* Table Entry     */
 while ( ddGetCurrent(&curRec) != 0 ){
  if ( curRec->isInNameList == 1 ) {
   if ( toupper(curRec->name[0]) != curLetter ){
    curLetter = toupper(curRec->name[0]);
    html_indexRecord(parmdictfd,curLetter);
    html_columnHeading(parmdictfd);

    /* Do sub listing for this letter and/or symbol */
    if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
    sprintf(vdictSubName,"%s/%s_pref%c%s",rootDir,PARMDICT,curLetter,HTMLSUF);
    vdictfd_sub = fopen(vdictSubName,"w");
    sprintf(vDictSubHeader,"Table of runtime parameters starting with \"%c\"",curLetter);
    vDictTitleWord=vDictSubHeader;
    vDictStart(vdictfd_sub);
    vDictTitleWord=NULL;
    html_columnHeading(vdictfd_sub);
   }

   /* Start row */
   fprintf(parmdictfd,"<TR>\n");
   fprintf(vdictfd_sub,"<TR>\n");

   /* Variable name column */
   vDictNameCol( parmdictfd,     curRec );
   vDictNameCol( vdictfd_sub, curRec );

   /* Variable def, units, refs column */
   vDictDefnCol( parmdictfd,     curRec );
   vDictDefnCol( vdictfd_sub, curRec );

   /* Variable use count */
   vDictUsesCol( parmdictfd,     curRec );
   vDictUsesCol( vdictfd_sub, curRec );

   /* End of row */
   fprintf(parmdictfd,"</TR>\n");
   fprintf(parmdictfd,"\n");
   fprintf(vdictfd_sub,"</TR>\n");
   fprintf(vdictfd_sub,"\n");
  }
 }

 if ( parmdictfd != NULL ) { 
   /* End last Table of uses */
   fprintf(parmdictfd,"</TABLE>");
   /* End document */
   fprintf(parmdictfd,"</HTML>\n");
   fclose(parmdictfd);
 }
 if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
}

GeneratePrecompDict()
/*
 Generate tables of compile time parameters
*/
{
 ddRecord *curRec;
 char *note;
 char curLetter = ' ';
 FILE *vdictfd_sub = NULL;
 char vdictSubName[MAXPATHNAM];
 char vDictSubHeader[1024];
 static char vDictHeader[] = "Alphabetic table of All Compile Time Parameters";

 fprintf(stderr,"Hi there \n");
 /* HTML headers */
 vDictTitleWord=vDictHeader;
 vDictStart(compdictfd);
 vDictTitleWord=NULL;

 /* Table Entry     */
 while ( ddGetCurrent(&curRec) != 0 ){
  if ( curRec->isInIfdef == 1 ) {
   if ( toupper(curRec->name[0]) != curLetter ){
    curLetter = toupper(curRec->name[0]);
    html_indexRecord(compdictfd,curLetter);
    html_columnHeading(compdictfd);

    /* Do sub listing for this letter and/or symbol */
    if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
    sprintf(vdictSubName,"%s/%s_pref%c%s",rootDir,COMPDICT,curLetter,HTMLSUF);
    vdictfd_sub = fopen(vdictSubName,"w");
    sprintf(vDictSubHeader,"Table of compile time parameters starting with \"%c\"",curLetter);
    vDictTitleWord=vDictSubHeader;
    vDictStart(vdictfd_sub);
    vDictTitleWord=NULL;
    html_columnHeading(vdictfd_sub);
   }

   /* Start row */
   fprintf(compdictfd,"<TR>\n");
   fprintf(vdictfd_sub,"<TR>\n");

   /* Variable name column */
   vDictNameCol( compdictfd,     curRec );
   vDictNameCol( vdictfd_sub, curRec );

   /* Variable def, units, refs column */
   vDictDefnCol( compdictfd,     curRec );
   vDictDefnCol( vdictfd_sub, curRec );

   /* Variable use count */
   vDictUsesCol( compdictfd,     curRec );
   vDictUsesCol( vdictfd_sub, curRec );

   /* End of row */
   fprintf(compdictfd,"</TR>\n");
   fprintf(compdictfd,"\n");
   fprintf(vdictfd_sub,"</TR>\n");
   fprintf(vdictfd_sub,"\n");
  }
 }

 if ( compdictfd != NULL ) { 
   /* End last Table of uses */
   fprintf(compdictfd,"</TABLE>");
   /* End document */
   fprintf(compdictfd,"</HTML>\n");
   fclose(compdictfd);
 }
 if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
}

GenerateProcDict()
/*
 Generate tables of procedures
*/
{
 ddRecord *curRec;
 char *note;
 char curLetter = ' ';
 FILE *vdictfd_sub = NULL;
 char vdictSubName[MAXPATHNAM];
 char vDictSubHeader[1024];
 static char vDictHeader[] = "Alphabetic table of all subroutines and functions";

 fprintf(stderr,"Hi there \n");
 /* HTML headers */
 vDictTitleWord=vDictHeader;
 vDictStart(procdictfd);
 vDictTitleWord=NULL;

 /* Table Entry     */
 while ( ddGetCurrent(&curRec) != 0 ) {
  if ( curRec->isProcName == 1 ) {
   if ( toupper(curRec->name[0]) != curLetter ){
    curLetter = toupper(curRec->name[0]);
    html_indexRecord(procdictfd,curLetter);
    html_columnHeading(procdictfd);

    /* Do sub listing for this letter and/or symbol */
    if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
    sprintf(vdictSubName,"%s/%s_pref%c%s",rootDir,PROCDICT,curLetter,HTMLSUF);
    vdictfd_sub = fopen(vdictSubName,"w");
    sprintf(vDictSubHeader,"Table of subroutines anf functions starting with \"%c\"",curLetter);
    vDictTitleWord=vDictSubHeader;
    vDictStart(vdictfd_sub);
    vDictTitleWord=NULL;
    html_columnHeading(vdictfd_sub);

   }

   /* Start row */
   fprintf(procdictfd,"<TR>\n");
   fprintf(vdictfd_sub,"<TR>\n");
 
   /* Variable name column */
   vDictNameCol( procdictfd,     curRec );
   vDictNameCol( vdictfd_sub, curRec );

   /* Variable def, units, refs column */
   vDictDefnCol( procdictfd,     curRec );
   vDictDefnCol( vdictfd_sub, curRec );

   /* Variable use count */
   vDictUsesCol( procdictfd,     curRec );
   vDictUsesCol( vdictfd_sub, curRec );

   /* End of row */
   fprintf(procdictfd,"</TR>\n");
   fprintf(procdictfd,"\n");
   fprintf(vdictfd_sub,"</TR>\n");
   fprintf(vdictfd_sub,"\n");
  }
 }

 if ( procdictfd != NULL ) { 
   /* End last Table of uses */
   fprintf(procdictfd,"</TABLE>");
   /* End document */
   fprintf(procdictfd,"</HTML>\n");
   fclose(procdictfd);
 }
 if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
}

GenerateVDict()
{
 ddRecord *curRec;
 char *note;
 char curLetter = ' ';
 FILE *vdictfd_sub = NULL;
 char vdictSubName[MAXPATHNAM];
 char vDictSubHeader[1024];
 static char vDictHeader[] = "Alphabetic table of all symbols";

 fprintf(stderr,"Hi there \n");
 /* HTML headers */
 vDictTitleWord=vDictHeader;
 vDictStart(vdictfd);
 vDictTitleWord=NULL;

 /* Table Entry     */
 while ( ddGetCurrent(&curRec) != 0 ){
  if ( toupper(curRec->name[0]) != curLetter ){
    curLetter = toupper(curRec->name[0]);
    html_indexRecord(vdictfd,curLetter);
    html_columnHeading(vdictfd);

    /* Do sub listing for this letter and/or symbol */
    if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }
    sprintf(vdictSubName,"%s/%s_pref%c%s",rootDir,"vdict",curLetter,HTMLSUF);
    vdictfd_sub = fopen(vdictSubName,"w");
    sprintf(vDictSubHeader,"Table of symbols starting with \"%c\"",curLetter);
    vDictTitleWord=vDictSubHeader;
    vDictStart(vdictfd_sub);
    vDictTitleWord=NULL;
    html_columnHeading(vdictfd_sub);

  }

  /* Start row */
  fprintf(vdictfd,"<TR>\n");
  fprintf(vdictfd_sub,"<TR>\n");

  /* Variable name column */
  vDictNameCol( vdictfd,     curRec );
  vDictNameCol( vdictfd_sub, curRec );

  /* Variable def, units, refs column */
  vDictDefnCol( vdictfd,     curRec );
  vDictDefnCol( vdictfd_sub, curRec );

  /* Variable use count */
  vDictUsesCol( vdictfd,     curRec );
  vDictUsesCol( vdictfd_sub, curRec );

  /* End of row */
  fprintf(vdictfd,"</TR>\n");
  fprintf(vdictfd,"\n");
  fprintf(vdictfd_sub,"</TR>\n");
  fprintf(vdictfd_sub,"\n");

 }

 if ( vdictfd != NULL ) { 
   /* End last Table of uses */
   fprintf(vdictfd,"</TABLE>");
   /* End document */
   fprintf(vdictfd,"</HTML>\n");
   fclose(vdictfd);
 }
 if ( vdictfd_sub != NULL ) { vDictClose(vdictfd_sub); }

}
GenerateVDictIndex()
{
 FILE *fd; char fName[MAXPATHNAM];
 char curLetter = ' ';
 ddRecord *curRec;

 /* HTML headers */
 sprintf(fName,"%s/%s%s",rootDir,"index",HTMLSUF);
 fd = fopen(fName,"w");
 fprintf(fd,"<HTML>"); fprintf(fd,"<HEAD>"); fprintf(fd,"</HEAD>");
 fprintf(fd,"<TITLE>Name Index</TITLE>\n");
 fprintf(fd,"<BODY text=\"#000000\" bgcolor=\"#FFFFFF\">");
 
 while ( ddGetCurrent(&curRec) != 0 ){
  if ( toupper(curRec->name[0]) != curLetter ){
   curLetter = toupper(curRec->name[0]);
   /*
   fprintf(fd,"<BR><FONT SIZE=8 ><B>%c</A></B></FONT>\n",curLetter);
   fprintf(fd,"<FONT SIZE=2 ><B> \n"); */
   fprintf(fd,"<BR><BR> \n");
   fprintf(fd,"</LI><LI> \n");
  } else {
   fprintf(fd,",\n");
  }
  if ( curRec->active > 0 ) {
   fprintf(fd,"<A HREF=%s/%s%s>",VARSUF,curRec->key,HTMLSUF);
   fprintf(fd,"%s</A>",curRec->name);
  } else {
   fprintf(fd,"%s",curRec->name);
  }
 }
 /* End document */
 fprintf(fd,"</BODY>\n");
 fprintf(fd,"</HTML>\n");
 fclose(fd);
 
}

int html_indexRecord(curFd, letter)
FILE *curFd;
char letter;
{
 int i;

 fprintf(curFd,"<TR>\n");
 fprintf(curFd," <TD COLSPAN=3>\n");
 fprintf(curFd,"  <FONT SIZE=8 ><B><A NAME=i_%c>%c</A></B></FONT>\n",letter,letter);
 fprintf(curFd,"  <FONT SIZE=2 ><B>\n");
 for (i=0;i<24;i=i+3){
  fprintf(curFd,"  <A HREF=#i_%c><U>%c</U></A>",'A'+i,'A'+i);
  fprintf(curFd," <A HREF=#i_%c><U>%c</U></A>",'A'+i+1,'A'+i+1);
  fprintf(curFd," <A HREF=#i_%c><U>%c</U></A>\n",'A'+i+2,'A'+i+2);
 }
 fprintf(curFd,"  <A HREF=#i_%c><U>%c</U></A>",'Y','Y');
 fprintf(curFd," <A HREF=#i_%c><U>%c</U></A>\n",'Z','Z');
 fprintf(curFd,"  </FONT></B>\n");
 fprintf(curFd," </TD>\n");
 fprintf(curFd,"</TR>\n");
 fprintf(curFd,"\n");
}

int html_columnHeading(FILE *curFd)
{
 fprintf(curFd,"<TR>\n");
 fprintf(curFd," <TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\"> ");
 fprintf(curFd,"  <FONT SIZE=5><B><U>Symbol </U></B></FONT>");
 fprintf(curFd," </TD>\n");
 fprintf(curFd," <TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\"> ");
 fprintf(curFd,"  <FONT SIZE=5><B><U>Description</U></B></FONT>");
 fprintf(curFd," </TD>\n");
 fprintf(curFd," <TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\"> ");
 fprintf(curFd,"  <FONT SIZE=5><B><U>Uses</U></B></FONT>");
 fprintf(curFd," </TD>\n");
 fprintf(curFd,"</TR>\n");
 fprintf(curFd,"\n");
}

int vDictClose(FILE *curFd)
{
     /* End last Table of uses */
     fprintf(curFd,"</TABLE>");
     /* End document */
     fprintf(curFd,"</BODY></HTML>\n");
     fclose(curFd);
}

int vDictStart(FILE *curFd)
{
 fprintf(curFd,"<HTML>\n");
 fprintf(curFd,"<HEAD>\n");
 fprintf(curFd,"</HEAD>\n");
 if ( vDictTitleWord == NULL ) vDictTitleWord = vDictDefaultTitleWord;
 fprintf(curFd,"<TITLE>%s</TITLE>\n",vDictTitleWord);
 fprintf(curFd,"<BODY text=\"#000000\" bgcolor=\"#FFFFFF\">");
 /* Begin table */
 fprintf(curFd,
 "<TABLE BORDER CELLSPACING=1 BORDERCOLOR=\"#000000\" CELLPADDING=2 >\n");
}

int vDictNameCol(FILE *vdictfd, ddRecord *curRec)
{
  if ( curRec->active > 0 ) {
   fprintf(vdictfd," <TD>\n  <B><FONT FACE=\"Courier\"><A HREF=%s/%s%s>   ",
           VARSUF,curRec->key,HTMLSUF);
   fprintf(vdictfd,"%s   </A></B></FONT>\n </TD>\n",curRec->name);
  } else {
   fprintf(vdictfd," <TD>\n  <B><FONT FACE=\"Courier\">");
   fprintf(vdictfd,"%s   </B></FONT>\n </TD>\n",curRec->name);
  }
}

vDictDefnCol(FILE *vdictfd, ddRecord *curRec )
{
  char *note;
  fprintf(vdictfd," <TD>\n");
  fprintf(vdictfd,"  <A NAME=var.%s>",curRec->name);
  if ( curRec->textEntry != NULL ) { 
   fprintf(vdictfd,"  %s</A>\n",curRec->textEntry);
  } else {
   fprintf(vdictfd,"  <B><U> </U></B></A>\n");
   /* fprintf(vdictfd,"  <B><U>%s %s %s %s</U></B></A>\n",
          "*** NO DEFINITION ***", "*** NO DEFINITION ***",
          "*** NO DEFINITION ***", "*** NO DEFINITION ***"); */
  }
  if ( curRec->unitsEntry != NULL && strlen(curRec->unitsEntry) != 0 ){
   fprintf(vdictfd,"  (<I>units</I>: %s ) \n",curRec->unitsEntry);
  }
  if ( curRec->hrefEntry != NULL && strlen(curRec->hrefEntry) != 0 ){
   fprintf(vdictfd,"  <sub><A HREF=%s><IMG SRC=\"OpenBookIcon.gif\"",curRec->hrefEntry);
   fprintf(vdictfd,"WIDTH=30 HEIGHT=15 ALT=\"Goto Manual\"></A></sub>\n");
  }
  if ( curRec->footNotesEntry != NULL ){
    fprintf(vdictfd,"  <sup><B>");
    note=strtok(curRec->footNotesEntry," ");
    fprintf(vdictfd,"<A HREF=#footnote_%s>%s</A>",note,note);
    while ( (note = strtok((char *)NULL," ")) != (char *)NULL ) {
     fprintf(vdictfd,"\n,  <A HREF=#footnote_%s>%s</A>",note,note);
    }
    fprintf(vdictfd,"</B></sup>\n");
  }
  fprintf(vdictfd," </TD>\n");
}

int vDictUsesCol(FILE *vdictfd, ddRecord *curRec)
{
  fprintf(vdictfd," <TD>\n");
  fprintf(vdictfd,"  %d\n",curRec->active);
  fprintf(vdictfd," </TD>\n");
}

    /* Record new entry in dd */
int addRecord()
{
  ddRecord rec; int recNo;
  rec.name           = curName;
  rec.hrefEntry      = curHref;
  rec.textEntry      = curText;
  rec.unitsEntry     = curUnits;
  rec.footNotesEntry = curFootNotes;
  rec.active         = 0;
  fprintf(stdout,"Adding DD record %s\n",rec.name);
  fflush(stdout);

  /* Make dd entry */
  ddAdd(&rec);

  /* Free temporary name string copies */
  if ( curName != NULL ) {
   fprintf(stdout,"Free %s\n",curName);fflush(stdout);
   free(curName);curName=NULL;
  }
  if ( curHref != NULL ) {
   fprintf(stdout,"Free %s\n",curHref);fflush(stdout);
   free(curHref);curHref=NULL;
  }
  if ( curText != NULL ) {
   fprintf(stdout,"Free %s\n",curText);fflush(stdout);
   free(curText);curText=NULL;
  }
  if ( curUnits != NULL ) {
   fprintf(stdout,"Free %s\n",curUnits);fflush(stdout);
   free(curUnits);curUnits=NULL;
  }
  if ( curFootNotes != NULL ) {
   fprintf(stdout,"Free %s\n",curFootNotes);fflush(stdout);
   free(curFootNotes);curFootNotes=NULL;
  }

  fprintf(stdout,"Added DD record \n");
  fflush(stdout);
}
    
     /* Join two strdup created strings and free the originals */
char *strjoin(s1, s2)
char *s1; char *s2;
{
  char *s;

  fprintf(stdout, "strjoin called \n");
  fflush(stdout);

  /* DEBUG - Die if both strings are NULL */
  /* if ( s1 == NULL && s2 == NULL ) exit(); */

  /* If either string is NULL return other */
  if ( s1 == NULL ) return(s2);
  if ( s2 == NULL ) return(s1);
 
  s = (char *)malloc(strlen(s1)+strlen(s2)+1);
  s = strcpy(s,s1);
  s = strcat(s,s2);
   
}

     /* Like strdup but changes \. into . */
char *strcopy(s1)
char *s1;
{
  char *s; int i;int l; int bs; int el;

  /* If string is NULL return. */
  if ( s1 == NULL ) return(s1);
  s = strdup(s1);
  return(s);
 
  l = strlen(s1)+1; bs = 0; el=0;
  s = (char *)malloc(l);
  if ( s == NULL ) return(s);
  for ( i=0;i<l;++i) {
   if ( bs == 0 ) {
    if ( s1[i] != '\\' ) {
     s[el]=s1[i];++el;
    }
    else {
     bs = 1;
    }
   } else {
    s[el]=s1[i];++el; bs=0;
   }
  }
  s[el]='\0';
  return(s);
}

int ParseF90Code()
    /* Read the source code */
{
  int i; FILE *file; int j;
  char tmpFileName[MAXPATHNAM];
  char srcFileName[MAXPATHNAM];
  char srcDirName[MAXPATHNAM];

  /* Create scratch file for loggin variable uses */
  sprintf(tmpFileName,"%s/%s",OUTDIR,TMP1);
  tmpfd = fopen(tmpFileName,"w");

  /* Parse the list of source files */
  if ( argList.nSFiles > 0 ) {
   for (i=1;i<=argList.nSFiles;++i){
    file = fopen(argList.srcFiles[i-1],"r");
    if (!file) { 
     fprintf(stderr,"Unable to open file \"%s\"\n",argList.srcFiles[i-1]); 
    } else {
     F90symin = file; 
     currentFile=argList.srcFiles[i-1];
     j=strlen(argList.srcFiles[i-1]);
     while ( j > 0 && *(argList.srcFiles[i-1]+j-1) != '/') --j;
     currentProcedure=NOPROC;
     Lno=1; Cno=1; 
     currentLineHtml[0]=(char)NULL;
     /* Create file for writing html source */
     ++nSrcFile;
     sprintf(sHtmlName,"%s/%d%s",srcDir,nSrcFile,HTMLSUF);
     srcfd=fopen(sHtmlName,"w");
     sprintf(sHtmlName,"%d%s",nSrcFile,HTMLSUF);
     fprintf(srcfd,"<HTML>\n");
     fprintf(srcfd,"<TITLE>%s</TITLE>\n",currentFile);
     fprintf(srcfd,"<BODY text=\"#000000\" bgcolor=\"#FFFFFF\">\n");
     fprintf(srcfd,"<PRE>\n");

     F90symparse();
     fprintf(srcfd,"</PRE></BODY></HTML>\n");
     fclose(srcfd);
     fclose(file); 
     /* Write directory, file and HTML src name */
     strncpy(srcDirName,argList.srcFiles[i-1],MAXPATHNAM-1);
     srcDirName[j]='\0';
     if ( j == 0 ) {
      srcDirName[0] = '.';
      srcDirName[1] = '/';
      srcDirName[2] = '\0';
     }
     strncpy(srcFileName,argList.srcFiles[i-1]+j,MAXPATHNAM-1);
     srcFileName[MAXPATHNAM]='\0';
     printf("HTM %s %s %s\n",
      srcDirName, srcFileName,sHtmlName);
     /* Add entry to the "FileDirectory" table. */
     fdAdd( srcDirName, srcFileName, sHtmlName );
    }
   }
   fdPrint();
  }
  else
  {   
   currentFile="STANDARD INPUT";currentLineHtml[0]=(char)NULL;
   F90symparse();
  }

  fclose(tmpfd);
}

int tblSort()
{
  /*
    use system() to sort tmp1 > tmp2.
    Sort removes multiple entries so that if a variable appears
    multiple times on a single line only one table entry will 
    be generated.
  */
  char command[500];
  sprintf(command,"sort %s/%s | uniq | grep -v '^ *$' > %s/%s",
   OUTDIR,TMP1,OUTDIR,TMP2);
  system(command);
}

int GenerateVarTables()
{
  char tmpFileName[MAXPATHNAM];
  char vdFileName[MAXPATHNAM];
  char ioBuff[currentLineHtmlSize*2];
  char *vNam, *vLast, *lineRef, *fileRef, *procRef, *codeRec;
  char *note;
  FILE *tabfd;
  ddRecord *curRec;
  ddRecord rec;

  sprintf(tmpFileName,"%s/%s",OUTDIR,TMP2);
  tabfd = fopen(tmpFileName,"r");
  vLast = NULL; vdictfd = NULL;
 
  while ( fscanf(tabfd,"%[^\n]%*[\n]",ioBuff) != EOF ) {
   /* Begin CNH debug */
   /* printf("ioBuff = \"%s\"\n",ioBuff); */
   /* exit(1); */
   /* End CNH debug */
   vNam    = strtok(ioBuff,",");
   /* Trick here that should be fixed.                    */
   /* Testing lineRef != NULL checks to see whether the   */
   /* there was a format error in the sorted file. The    */
   /* file will only have an error if the parsing rules   */
   /* are incomplete.                                     */
   if ( vNam != NULL ) {
   lineRef = strtok(NULL,",");
   fileRef = strtok(NULL,",");
   procRef = strtok(NULL,",");
   codeRec = procRef+strlen(procRef)+1;
   if ( vLast == NULL || strcmp(vNam,vLast) != 0 ) {
    if ( vLast != NULL ){ free(vLast);}
    vLast = strdup(vNam);
    rec.name = vNam;
    curRec = ddFind(&rec);
    if ( curRec == NULL ) {
     printf("Variable %s NOT FOUND\n",vNam);
     exit(1);
    } else {
     printf("New variable %s key = %s\n",vNam,curRec->key);
    }
    if ( vdictfd != NULL ){ 
     /* End Table of uses */
     fprintf(vdictfd,"</TABLE>");
     /* End document */
     fprintf(vdictfd,"</HTML>\n");
     fclose(vdictfd);
    }
    sprintf(vdFileName,"%s/%s/%s%s",OUTDIR,VARSUF,curRec->key,HTMLSUF);
    vdictfd = fopen(vdFileName,"w");
    /* HTML headers */
    fprintf(vdictfd,"<HTML>\n");
    fprintf(vdictfd,"<HEAD>\n");
    fprintf(vdictfd,"</HEAD>\n");
    fprintf(vdictfd,"<TITLE>Table showing occurences of \"%s\"</TITLE>\n",curRec->name);
    fprintf(vdictfd,"<BODY text=\"#000000\" bgcolor=\"#FFFFFF\">\n");
    /* Begin table */
    fprintf(vdictfd,
    "<TABLE BORDER CELLSPACING=1 BORDERCOLOR=\"#000000\" CELLPADDING=2 >\n");
    html_columnHeading(vdictfd);
    fprintf(vdictfd,"<TR>\n");
    fprintf(vdictfd," <TD>\n  <B><FONT FACE=\"Courier\">");
    fprintf(vdictfd,"%s   </B></FONT>\n </TD>\n",curRec->name);
    fprintf(vdictfd," <TD>\n");
    fprintf(vdictfd,"  <A NAME=var.%s>",curRec->name);
    if ( curRec->textEntry != NULL ) { 
     fprintf(vdictfd,"  %s</A>\n",curRec->textEntry);
    } 
    else 
    {
     fprintf(vdictfd,"  <B><U> </U></B></A>\n");
/*   fprintf(vdictfd,"  <B><U>%s %s %s %s</U></B></A>\n",
            "*** NO DEFINITION ***", "*** NO DEFINITION ***",
            "*** NO DEFINITION ***", "*** NO DEFINITION ***"); */
    }
    if ( curRec->unitsEntry != NULL && strlen(curRec->unitsEntry) != 0 ){
     fprintf(vdictfd,"  (<I>units</I>: %s ) \n",curRec->unitsEntry);
    }
    if ( curRec->hrefEntry != NULL && strlen(curRec->hrefEntry) != 0 ){
     fprintf(vdictfd,"  <sub><A HREF=%s><IMG SRC=\"OpenBookIcon.gif\"",curRec->hrefEntry);
     fprintf(vdictfd,"WIDTH=30 HEIGHT=15 ALT=\"Goto Manual\"></A></sub>\n");
    }
    if ( curRec->footNotesEntry != NULL ){
     fprintf(vdictfd,"  <sup><B>");
     note=strtok(curRec->footNotesEntry," ");
     fprintf(vdictfd,"<A HREF=#footnote_%s>%s</A>",note,note);
     while ( (note = strtok((char *)NULL," ")) != (char *)NULL ) {
      fprintf(vdictfd,"\n,  <A HREF=#footnote_%s>%s</A>",note,note);
     }
     fprintf(vdictfd,"</B></sup>\n");
    }
    fprintf(vdictfd," </TD>\n");
    fprintf(vdictfd," <TD>\n");
    fprintf(vdictfd,"  %d\n",curRec->active);
    fprintf(vdictfd," </TD>\n");
    fprintf(vdictfd,"</TR>\n");
    fprintf(vdictfd,"\n");
    /* End header table */
    fprintf(vdictfd,"</TABLE>\n<BR><HR><BR>\n");

    /* Begin table of uses */
    fprintf(vdictfd,
    "<TABLE BORDER CELLSPACING=1 BORDERCOLOR=\"#000000\" CELLPADDING=2 WIDTH=900>\n");
    fprintf(vdictfd,"<TR>\n<TD>File</TD>\n<TD>Line number</TD>\n<TD>Procedure</TD>\n<TD>Code</TD>\n</TR>\n");
   }
   /* Table of uses */
   fprintf(vdictfd,"<TR>\n");
   fprintf(vdictfd," <TD>\n");
   fprintf(vdictfd,"  %s",fileRef);
   fprintf(vdictfd," </TD>\n");
   fprintf(vdictfd," <TD>\n");
   fprintf(vdictfd,"  %s",lineRef);
   fprintf(vdictfd," </TD>\n");
   fprintf(vdictfd," <TD>\n");
   fprintf(vdictfd,"  %s",procRef);
   fprintf(vdictfd," </TD>\n");
   fprintf(vdictfd," <TD>\n");
   fprintf(vdictfd,"  <PRE>%s</PRE>",codeRec);
   fprintf(vdictfd," </TD>\n");
   fprintf(vdictfd,"</TR>\n");
  }
 }
 if ( vdictfd != NULL ){ 
   /* End last Table of uses */
   fprintf(vdictfd,"</TABLE>");
   /* End document */
   fprintf(vdictfd,"</BODY></HTML>\n");
   fclose(vdictfd);
  }
}
