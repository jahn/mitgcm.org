/*
$Id: index.h,v 1.52 2002/08/29 13:45:39 jmruiz Exp $
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
*/

#ifndef __HasSeenModule_Index
#define __HasSeenModule_Index       1

struct dev_ino
{
    dev_t   dev;
    ino_t   ino;
    struct dev_ino *next;
};

struct IgnoreLimitPositions
{
    int     n;                  /* Number of entries per file */
    int    *pos;                /* Store metaID1,position1, metaID2,position2 ..... */
};

/* This is used to build a list of the metaIDs that are currently in scope when indexing words */

typedef struct
{
    int    *array;              /* list of metaIDs that need to be indexed */
    int     max;                /* max size of table */
    int     num;                /* number in list */
    int     defaultID;          /* default metaID (should always be one, I suppose) */
}
METAIDTABLE;


/*
   -- module data
*/


struct MOD_Index
{
    /* entry vars */
    METAIDTABLE metaIDtable;
    ENTRYARRAY *entryArray;
    ENTRY  *hashentries[VERYBIGHASHSIZE];
    char    hashentriesdirty[VERYBIGHASHSIZE]; /* just a 0/1 flag */

    /* Compression Work buffer while compression locations in index ** proccess */
    unsigned char *compression_buffer;
    int     len_compression_buffer;

    unsigned char *worddata_buffer;  /* Buffer to store worddata */
    int    len_worddata_buffer;     /* Max size of the buffer */
    int    sz_worddata_buffer;      /* Space being used in worddata_buffer */

    /* File counter */
    int     filenum;

    /* index tmp (both FS and HTTP methods) */
    char   *tmpdir;

    /* Filenames of the swap files */
    char   *swap_location_name[MAX_LOC_SWAP_FILES]; /* Location info file */

    /* handlers for both files */
    FILE   *fp_loc_write[MAX_LOC_SWAP_FILES];       /* Location (writing) */
    FILE   *fp_loc_read[MAX_LOC_SWAP_FILES];        /* Location (reading) */

    struct dev_ino *inode_hash[BIGHASHSIZE];

    /* Buffers used by indexstring */
    int     lenswishword;
    char   *swishword;
    int     lenword;
    char   *word;

    /* Economic mode (-e) */
    int     swap_locdata;       /* swap location data */

    /* Pointer to swap functions */
    long    (*swap_tell) (FILE *);
            size_t(*swap_write) (const void *, size_t, size_t, FILE *);
    int     (*swap_seek) (FILE *, long, int);
            size_t(*swap_read) (void *, size_t, size_t, FILE *);
    int     (*swap_close) (FILE *);
    int     (*swap_putc) (int, FILE *);
    int     (*swap_getc) (FILE *);

    /* IgnoreLimit option values */
    int     plimit;
    int     flimit;
    /* Number of words from IgnoreLimit */
    int     nIgnoreLimitWords;
    /* Positions from stopwords from IgnoreLimit */
    struct IgnoreLimitPositions **IgnoreLimitPositionsArray;

    /* Index in blocks of chunk_size files */
    int     chunk_size;

    /* Variable to control the size of the zone used for store locations during chunk proccesing */
    int     optimalChunkLocZoneSize;

    /* variable to handle free memory space for locations inside currentChunkLocZone */

    LOCATION *freeLocMemChain;

    MEM_ZONE *perDocTmpZone;
    MEM_ZONE *currentChunkLocZone;
    MEM_ZONE *totalLocZone;
    MEM_ZONE *entryZone;
};

void    initModule_Index(SWISH *);
void    freeModule_Index(SWISH *);
int     configModule_Index(SWISH *, StringList *);


void    do_index_file(SWISH * sw, FileProp * fprop);

ENTRY  *getentry(SWISH * , char *);
void    addentry(SWISH *, ENTRY *, int, int, int, int);

void    addCommonProperties(SWISH * sw, FileProp * fprop, FileRec * fi, char *title, char *summary, int start);


int     getfilecount(IndexFILE *);

int     getNumberOfIgnoreLimitWords(SWISH *);
void    getPositionsFromIgnoreLimitWords(SWISH * sw);

char   *ruleparse(SWISH *, char *);
void    stripIgnoreFirstChars(INDEXDATAHEADER *, char *);
void    stripIgnoreLastChars(INDEXDATAHEADER *, char *);

#define isIgnoreFirstChar(header,c) (header)->ignorefirstcharlookuptable[(int)((unsigned char)c)]
#define isIgnoreLastChar(header,c) (header)->ignorelastcharlookuptable[(int)((unsigned char)c)]
#define isBumpPositionCounterChar(header,c) (header)->bumpposcharslookuptable[(int)((unsigned char)c)]


void    computehashentry(ENTRY **, ENTRY *);

void    sort_words(SWISH *, IndexFILE *);
void    sortChunkLocations(SWISH *, IndexFILE *, ENTRY *);

int     indexstring(SWISH * sw, char *s, int filenum, int structure, int numMetaNames, int *metaID, int *position);

void    addsummarytofile(IndexFILE *, int, char *);

void    BuildSortedArrayOfWords(SWISH *, IndexFILE *);



void    PrintHeaderLookupTable(int ID, int table[], int table_size, FILE * fp);
void    coalesce_all_word_locations(SWISH * sw, IndexFILE * indexf);
void    coalesce_word_locations(SWISH * sw, IndexFILE * indexf, ENTRY * e);

void    adjustWordPositions(unsigned char *worddata, int *sz_worddata, int n_files, struct IgnoreLimitPositions **ilp);

#endif
