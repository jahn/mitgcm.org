/*
$Id: filter.h,v 1.10 2002/03/17 03:54:19 whmoseley Exp $
**
** This program and library is free software; you can redistribute it and/or
** modify it under the terms of the GNU (Library) General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU (Library) General Public License for more details.
**
** You should have received a copy of the GNU (Library) General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**
** 2001-02-28 rasc    own module started for filters
** 2001-04-09 rasc    enhancing filters
*/


#ifndef __HasSeenModule_Filter
#define __HasSeenModule_Filter	1


/* Module data and structures */



typedef struct FilterList  /* 2002-03-16 moseley */
{
    struct FilterList   *next;
    char                *prog;      /* program name to run */
    char                *options;   /* options list */
    regex_list          *regex;     /* list of regular expressions */
    char                *suffix;    /* or plain text suffix */
} FilterList;




/* Global module data */

struct MOD_Filter {
   /* public:  */
   /* none */
   
   /* private: don't use outside this module! */
    char   *filterdir;              /* 1998-08-07 rasc */ /* depreciated */
    FilterList *filterlist;  /* 2002-03-16 moseley */

};





/* exported Prototypes */

void initModule_Filter   (SWISH *sw);
void freeModule_Filter   (SWISH *sw);
int  configModule_Filter (SWISH *sw, StringList *sl);

struct FilterList *hasfilter (SWISH *sw, char *filename);
FILE *FilterOpen (FileProp *fprop);
int FilterClose (FILE *fp);


#endif


