/*
$Id: html.h,v 1.15 2001/10/11 22:21:13 whmoseley Exp $

*/

/*
   seems to be a very old module of swish
   some serious work to do in html.c and html.h!! 
   but anyway it seems to work...
 */


/* Just the prototypes */

int countwords_HTML(SWISH *sw, FileProp *fprop, FileRec *fi, char *buffer);

int parsecomment (SWISH *, char *, int, int, int, int *);
char   *parseHTMLtitle(SWISH *,char *buffer);
int     isoktitle(SWISH *sw, char *title);
