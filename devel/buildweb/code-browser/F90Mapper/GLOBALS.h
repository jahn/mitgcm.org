/* $Id: GLOBALS.h,v 1.2 1997/03/23 05:52:43 cnh Exp cnh $ */

char *strjoin();
char *strcopy();

     /* Current input file */
char *currentFile;          

     /* Current subroutine name and start line number. */
char *currentProcedure;          
int  currentProcedureLine0;          
#define NOPROC "NO PROCEDURE"

        /* Buffer for current line */
#define currentLineBufSize 500       
char currentLineText[currentLineBufSize];
        /* Buffer for current line with HTML markup */
#define currentLineHtmlSize 5000      
char currentLineHtml[currentLineHtmlSize];

        /* Output directory tree */
#define OUTDIR   "vdb"              
#define VARSUF   "names"
#define SRCSUF   "code"
#define VDICT    "vdict.htm"
#define PROCDICT "procdict"
#define PARMDICT "parmdict"
#define COMPDICT "compdict"
#define SFDICT   "sfdict"
#define TMP1     "tmp1"    
#define TMP2     "tmp2"    
#define HTMLSUF  ".htm"
#define MAXPATHNAM 1024
char *rootDir;
char *varDir;
char *srcDir;                     

     /* Output files */
FILE *vdictfd;        /* Dictionary            */
FILE *procdictfd;     /* Procedure Table       */
FILE *parmdictfd;     /* Runtime Param Table   */
FILE *compdictfd;     /* Compile Time Table    */
FILE *srcfd;          /* Source code           */
char sHtmlName[MAXPATHNAM]; /* Name            */
FILE *varfd;          /* Use table             */
FILE *tmpfd;          /* Scratch file          */

    /* Counters */
int Lno;             /* Current record number */
int Cno;             /* Current character number */

    /* State information */
int inNameList;
int inIfdef;
int nameIsProcName;

/* Variables used by parser to store data in processing. */
char *curName;
char *curHref;
char *curText;
char *curFootNotes;
char *curUnits;

char *base36(/* int n */);
