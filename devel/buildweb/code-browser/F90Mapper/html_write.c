/*
 Generic HTML writing functions
*/
#include <strings.h>
#include <stdio.h>
#include "DD.h"
#include "FD.h"
#include "GLOBALS.h"

html_ul( FILE *o) { fprintf(o,"<ul>" ); }
html_eul(FILE *o) { fprintf(o,"</ul>"); }
html_li( FILE *o) { fprintf(o,"<li>" ); }
html_eli(FILE *o) { fprintf(o,"</li>"); }
html_hn( FILE *o,char *h) { fprintf(o,"<%s>",h ); }
html_ehn(FILE *o,char *h) { fprintf(o,"</%s>",h); }

html_start(FILE *o){
 /* Generic header stuff */
 fprintf(o,"<HTML><HEAD>\n");
 fprintf(o,"<TITLE> </TITLE>\n");
 fprintf(o,"</HEAD>\n<BODY text=\"#000000\" bgcolor=\"#FFFFFF\">\n");
}

html_end(FILE *o){
 /* Generic footer stuff */
 fprintf(o,"</BODY></HTML>");
}

html_entryli(FILE *o,char *string,char *href,char *heading){
 /*
   do <li><"heading"><a href="href">"string"</a></"heading"></li>
 */
 html_li(o);
  fprintf(o,"<A href=%s>",href);
   html_hn(o,heading);
    fprintf(o,"%s",string);
   html_ehn(o,heading);
  fprintf(o,"</A>");
 html_eli(o);
}

html_entry(FILE *o,char *string,char *href){
 /*
   do <a href="href">"string"</a>
 */
 fprintf(o,"<A href=%s>",href);
 fprintf(o,"%s",string);
 fprintf(o,"</A>");
}

int html_FlistTabStart(FILE *curFd)
{
 /* fprintf(curFd,"<TITLE>Source Files</TITLE>"); */
 /* Begin table */
 fprintf(curFd,
 "<TABLE BORDER CELLSPACING=1 BORDERCOLOR=\"#000000\" CELLPADDING=2 >");
}

int html_FlistColHeader(FILE *curFd)
 /*  
   File listing table column format
 */
{
 fprintf(curFd,"<TR>");
 fprintf(curFd,"<TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\">");
 fprintf(curFd,"<FONT SIZE=5><B><U>Directory </U></B></FONT>");
 fprintf(curFd,"</TD>");
 fprintf(curFd,"<TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\">");
 fprintf(curFd,"<FONT SIZE=5><B><U>File Name</U></B></FONT>");
 fprintf(curFd,"</TD>");
 fprintf(curFd,"<TD VALIGN=\"CENTER\" HEIGHT=40 ALIGN=\"CENTER\">");
 fprintf(curFd,"<FONT SIZE=5><B><U>Description</U></B></FONT>");
 fprintf(curFd,"</TD>");
 fprintf(curFd,"</TR>");
}

int html_FlistColRecord(FILE *curFd, char *dirNam, char *fNam, char *hfNam, char *descrip)
 /*  
   File listing table column format
 */
{
 fprintf(curFd,"<TR>");
 fprintf(curFd,"<TD><B><FONT FACE=\"Courier\">");
 fprintf(curFd,"%s",dirNam);
 fprintf(curFd,"</FONT></B></TD>");
 fprintf(curFd,"<TD><B><FONT FACE=\"Courier\">");
 fprintf(curFd,"<A HREF=%s/%s>",SRCSUF,hfNam);
 fprintf(curFd,"%s",fNam);
 fprintf(curFd,"</A>");
 fprintf(curFd,"</FONT></B></TD>");
 fprintf(curFd,"<TD><B><FONT FACE=\"Courier\">");
 fprintf(curFd,"%s",descrip);
 fprintf(curFd,"</FONT></B></TD>");
 fprintf(curFd,"</TR>");
}

int html_FlistTabStop(FILE *curFd)
{
 /* End table */
 fprintf(curFd,
 "</TABLE>");
}
