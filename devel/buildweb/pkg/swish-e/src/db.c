/*
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
**
** 2001-05-07 jmruiz init coding
**
*/

#include "swish.h"
#include "mem.h"
#include "string.h"
#include "index.h"
#include "hash.h"
#include "date_time.h"
#include "compress.h"
#include "error.h"
#include "metanames.h"
#include "db.h"
#include "db_native.h"
// #include "db_berkeley_db.h"

#ifndef min
#define min(a, b)    (a) < (b) ? a : b
#endif

/*
  -- init structures for this module
*/

void initModule_DB (SWISH  *sw)
{
          /* Allocate structure */
   initModule_DBNative(sw);
   // initModule_DB_db(sw);
   return;
}


/*
  -- release all wired memory for this module
*/

void freeModule_DB (SWISH *sw)
{
  freeModule_DBNative(sw);
  // freeModule_DB_db(sw);
  return;
}



/* ---------------------------------------------- */



/*
 -- Config Directives
 -- Configuration directives for this Module
 -- return: 0/1 = none/config applied
*/

int configModule_DB  (SWISH *sw, StringList *sl)
{
 //struct MOD_DB *DB = sw->Db;
 // char *w0    = sl->word[0];
 int  retval = 1;


  retval = 0; // tmp due to empty routine

  return retval;
}


/* General write DB routines - Common to all DB */

/* Header routines */

#define write_header_int(sw,id,num,DB) {unsigned long itmp = (num); itmp = PACKLONG(itmp); DB_WriteHeaderData((sw),(id), (unsigned char *)&itmp, sizeof(long), (DB));}
#define write_header_int2(sw,id,num1,num2,DB) {unsigned long itmp[2]; itmp[0] = (num1); itmp[1] = (num2); itmp[0]=  PACKLONG(itmp[0]); itmp[1] = PACKLONG(itmp[1]); DB_WriteHeaderData((sw),(id), (unsigned char *)itmp, sizeof(long) * 2, (DB));}


void    write_header(SWISH *sw, INDEXDATAHEADER * header, void * DB, char *filename, int totalwords, int totalfiles, int merged)
{
    char   *c,
           *tmp;

    c = (char *) strrchr(filename, '/');
    if (!c || (c && !*(c + 1)))
        c = filename;
    else
        c += 1;

    DB_InitWriteHeader(sw, DB);

    DB_WriteHeaderData(sw, INDEXHEADER_ID, (unsigned char *)INDEXHEADER, strlen(INDEXHEADER) +1, DB);
    DB_WriteHeaderData(sw, INDEXVERSION_ID, (unsigned char *)INDEXVERSION, strlen(INDEXVERSION) + 1, DB);
    write_header_int(sw, MERGED_ID, merged, DB);
    DB_WriteHeaderData(sw, NAMEHEADER_ID, (unsigned char *)header->indexn, strlen(header->indexn) + 1, DB);
    DB_WriteHeaderData(sw, SAVEDASHEADER_ID, (unsigned char *)c, strlen(c) + 1, DB);
    write_header_int2(sw, COUNTSHEADER_ID, totalwords, totalfiles, DB);
    tmp = getTheDateISO(); 
    DB_WriteHeaderData(sw, INDEXEDONHEADER_ID, (unsigned char *)tmp, strlen(tmp) + 1,DB); 
    efree(tmp);
    DB_WriteHeaderData(sw, DESCRIPTIONHEADER_ID, (unsigned char *)header->indexd, strlen(header->indexd) + 1, DB);
    DB_WriteHeaderData(sw, POINTERHEADER_ID, (unsigned char *)header->indexp, strlen(header->indexp) + 1, DB);
    DB_WriteHeaderData(sw, MAINTAINEDBYHEADER_ID, (unsigned char *)header->indexa, strlen(header->indexa) + 1,DB);
    write_header_int(sw, DOCPROPENHEADER_ID, 1, DB);

    write_header_int(sw, FUZZYMODEHEADER_ID, header->fuzzy_mode, DB);

    write_header_int(sw, IGNORETOTALWORDCOUNTWHENRANKING_ID, header->ignoreTotalWordCountWhenRanking, DB);
    DB_WriteHeaderData(sw, WORDCHARSHEADER_ID, (unsigned char *)header->wordchars, strlen(header->wordchars) + 1, DB);
    write_header_int(sw, MINWORDLIMHEADER_ID, header->minwordlimit, DB);
    write_header_int(sw, MAXWORDLIMHEADER_ID, header->maxwordlimit, DB);
    DB_WriteHeaderData(sw, BEGINCHARSHEADER_ID, (unsigned char *)header->beginchars, strlen(header->beginchars) + 1, DB);
    DB_WriteHeaderData(sw, ENDCHARSHEADER_ID, (unsigned char *)header->endchars, strlen(header->endchars) + 1, DB);
    DB_WriteHeaderData(sw, IGNOREFIRSTCHARHEADER_ID, (unsigned char *)header->ignorefirstchar, strlen(header->ignorefirstchar) + 1, DB);
    DB_WriteHeaderData(sw, IGNORELASTCHARHEADER_ID, (unsigned char *)header->ignorelastchar, strlen(header->ignorelastchar) + 1,DB);
    /* Removed - Patents 
    write_header_int(FILEINFOCOMPRESSION_ID, header->applyFileInfoCompression, DB);
    */



    /* Jose Ruiz 06/00 Added this line to delimite the header */
    write_integer_table_to_header(sw, TRANSLATECHARTABLE_ID, header->translatecharslookuptable, sizeof(header->translatecharslookuptable) / sizeof(int), DB);
    
    /* Other header stuff */
        /* StopWords */
    write_words_to_header(sw, STOPWORDS_ID, header->hashstoplist, DB);
        /* Metanames */
    write_MetaNames(sw, METANAMES_ID, header, DB);

        /* BuzzWords */
    write_words_to_header(sw, BUZZWORDS_ID, header->hashbuzzwordlist, DB);

#ifndef USE_BTREE
    /* Write the total words per file array, if used */
    if ( !header->ignoreTotalWordCountWhenRanking )
        write_integer_table_to_header(sw, TOTALWORDSPERFILE_ID, header->TotalWordsPerFile, totalfiles, DB);
#endif

    DB_EndWriteHeader(sw, DB);
}


/* Jose Ruiz 11/00
** Function to write a word to the index DB
*/
void    write_word(SWISH * sw, ENTRY * ep, IndexFILE * indexf)
{
    long    wordID;

    wordID = DB_GetWordID(sw, indexf->DB);

    DB_WriteWord(sw, ep->word,wordID,indexf->DB);
        /* Store word offset for futher hash computing */
    ep->u1.wordID = wordID;

}

#ifdef USE_BTREE
/* 04/2002 jmruiz
** Routine to update wordID
*/
void    update_wordID(SWISH * sw, ENTRY * ep, IndexFILE * indexf)
{
    long    wordID;

    wordID = DB_GetWordID(sw, indexf->DB);

    DB_UpdateWordID(sw, ep->word,wordID,indexf->DB);
        /* Store word offset for futher hash computing */
    ep->u1.wordID = wordID;
}

void    delete_worddata(SWISH * sw, long wordID, IndexFILE * indexf)
{
    DB_DeleteWordData(sw,wordID,indexf->DB);
}

#endif

/* Jose Ruiz 11/00
** Function to write all word's data to the index DB
*/


void build_worddata(SWISH * sw, ENTRY * ep, IndexFILE * indexf)
{
    int     i, j,
            curmetaID,
            sz_worddata;
    unsigned long    tmp,
            curmetanamepos;
    int     metaID;
    int     bytes_size,
            chunk_size;
    unsigned char *compressed_data,
           *p,*q;
    LOCATION *l, *next;


    curmetaID=0;
    curmetanamepos=0L;
    q=sw->Index->worddata_buffer;

        /* Compute bytes required for chunk location size. Eg: 4096 -> 2 bytes, 65535 -> 2 bytes */
    for(bytes_size = 0, i = COALESCE_BUFFER_MAX_SIZE; i; i >>= 8)
        bytes_size++;

    /* Write tfrequency */
    q = compress3(ep->tfrequency,q);

    /* Write location list */
    for(l=ep->allLocationList;l;)
    {
        compressed_data = (unsigned char *) l;
            /* Get next element */
        next = *(LOCATION **)compressed_data;
            /* Jump pointer to next element */
        p = compressed_data + sizeof(LOCATION *);

        metaID = uncompress2(&p);

        for(chunk_size = 0, i = 0, j = bytes_size - 1; i < bytes_size; i++, j--)
            chunk_size |= p[i] << (j * 8);
        p += bytes_size;

        if(curmetaID!=metaID) 
        {
            if(curmetaID) 
            {
                /* Write in previous meta (curmetaID)
                ** file offset to next meta */
                tmp=q - sw->Index->worddata_buffer;
                PACKLONG2(tmp,sw->Index->worddata_buffer+curmetanamepos);
            }
                /* Check for enough memory */
                /* 
                ** MAXINTCOMPSIZE is for the worst case metaID
                **
                ** sizeof(long) is to leave four bytes to
                ** store the offset of the next metaname
                ** (it will be 0 if no more metanames).
                **
                ** 1 is for the trailing '\0'
                */

            tmp=q - sw->Index->worddata_buffer;

            if((long)(tmp + MAXINTCOMPSIZE + sizeof(long) + 1) >= (long)sw->Index->len_worddata_buffer)
            {
                sw->Index->len_worddata_buffer=sw->Index->len_worddata_buffer*2+MAXINTCOMPSIZE+sizeof(long)+1;
                sw->Index->worddata_buffer=(unsigned char *) erealloc(sw->Index->worddata_buffer,sw->Index->len_worddata_buffer);
                q=sw->Index->worddata_buffer+tmp;   /* reasign pointer inside buffer */
            }

                /* store metaID in buffer */
            curmetaID=metaID;
            q = compress3(curmetaID,q);

                /* preserve position for offset to next
                ** metaname. We do not know its size
                ** so store it as a packed long */ 
            curmetanamepos=q - sw->Index->worddata_buffer;

                /* Store 0 and increase pointer */
            tmp=0L;
            PACKLONG2(tmp,q);

            q+=sizeof(unsigned long);
        }
        /* Store all data for this chunk */
        /* First check for enough space 
        **
        ** 1 is for the trailing '\0'
        */

        tmp=q - sw->Index->worddata_buffer;

        if((long)(tmp + chunk_size + 1) >= (long)sw->Index->len_worddata_buffer)
        {
            sw->Index->len_worddata_buffer=sw->Index->len_worddata_buffer*2+chunk_size+1;
            sw->Index->worddata_buffer=(unsigned char *) erealloc(sw->Index->worddata_buffer,sw->Index->len_worddata_buffer);
            q=sw->Index->worddata_buffer+tmp;   /* reasign pointer inside buffer */
        }

        /* Copy it and advance pointer */
        memcpy(q,p,chunk_size);
        q += chunk_size;

        /* End of chunk mark -> Write trailing '\0' */
        *q++ = '\0';

        l = next;
    }

    /* Write in previous meta (curmetaID)
    ** file offset to end of metas */
    tmp=q - sw->Index->worddata_buffer;
    PACKLONG2(tmp,sw->Index->worddata_buffer+curmetanamepos);

    sz_worddata = q - sw->Index->worddata_buffer;

    /* Adjust word positions. 
    ** if ignorelimit was set and some new stopwords weee found, positions
    ** are recalculated
    ** Also call it even if we have not set IgnoreLimit to calesce word chunks
    ** and remove trailing 0 from chunks to save some bytes
    */
    adjustWordPositions(sw->Index->worddata_buffer, &sz_worddata, sw->indexlist->header.totalfiles, sw->Index->IgnoreLimitPositionsArray);

    sw->Index->sz_worddata_buffer = sz_worddata;
}

/* 04/2002 jmruiz
** New simpler routine to write worddata
*/
void write_worddata(SWISH * sw, ENTRY * ep, IndexFILE * indexf )
{
    DB_WriteWordData(sw, ep->u1.wordID,sw->Index->worddata_buffer,sw->Index->sz_worddata_buffer,indexf->DB);

}


/* 04/2002 jmruiz
** Function to read all word's data from the index DB
*/


long read_worddata(SWISH * sw, ENTRY * ep, IndexFILE * indexf, unsigned char **buffer, int *sz_buffer)
{
long wordID;
char *word = ep->word;

    DB_InitReadWords(sw, indexf->DB);
    DB_ReadWordHash(sw, word, &wordID, indexf->DB);

    if(!wordID)
    {    
        DB_EndReadWords(sw, indexf->DB);
        sw->lasterror = WORD_NOT_FOUND;
        *buffer = NULL;
        *sz_buffer = 0;
        return 0L;
   } 
   DB_ReadWordData(sw, wordID, buffer, sz_buffer, indexf->DB);
   DB_EndReadWords(sw, indexf->DB);
   return wordID;
}

/* 04/2002 jmruiz
** Routine to merge two buffers of worddata
*/
void add_worddata(SWISH *sw, ENTRY *epi, IndexFILE *indexf, unsigned char *olddata, int sz_olddata)
{
int maxtotsize;
unsigned char stack_buffer[32000];  /* Just to try malloc/free fragmentation */
unsigned char *newdata;
int sz_newdata;
int tfreq1, tfreq2;
unsigned char *p1, *p2, *p;
int curmetaID_1,curmetaID_2;
unsigned long nextposmetaname_1,nextposmetaname_2, curmetanamepos, curmetanamepos_1, curmetanamepos_2, tmp;
int last_filenum, filenum, tmpval, frequency, *posdata;
#define POSDATA_STACK 2000
int stack_posdata[POSDATA_STACK];  /* Just to avoid the overhead of malloc/free */
unsigned char r_flag, *w_flag;
unsigned char *q;

    /* First of all, ckeck for size in buffer */
    maxtotsize = sw->Index->sz_worddata_buffer + sz_olddata;
    if(maxtotsize > sw->Index->len_worddata_buffer)
    {
         sw->Index->len_worddata_buffer = maxtotsize + 2000;
         sw->Index->worddata_buffer = (unsigned char *) erealloc(sw->Index->worddata_buffer,sw->Index->len_worddata_buffer); 
    }
    /* Preserve new data in a local copy - sw->Index->worddata_buffer is the final destination
    ** of data
    */
    if(sw->Index->sz_worddata_buffer > sizeof(stack_buffer))
        newdata = (unsigned char *) emalloc(sw->Index->sz_worddata_buffer);
    else
        newdata = stack_buffer;
    sz_newdata = sw->Index->sz_worddata_buffer;
    memcpy(newdata,sw->Index->worddata_buffer, sz_newdata);

    /* Set pointers to all buffers */
    p1 = olddata;
    p2 = newdata; 
    q = p = sw->Index->worddata_buffer;

    /* Now read tfrequency */
    tfreq1 = uncompress2(&p1); /* tfrequency - number of files with this word */
    tfreq2 = uncompress2(&p2); /* tfrequency - number of files with this word */
    /* Write tfrequency */
    p = compress3(tfreq1 + tfreq2, p);

    /* Now look for MetaIDs */
    curmetaID_1 = uncompress2(&p1);
    curmetaID_2 = uncompress2(&p2);
    nextposmetaname_1 = UNPACKLONG2(p1); 
    p1 += sizeof(long);
    curmetanamepos_1 = p1 - olddata;
    nextposmetaname_2 = UNPACKLONG2(p2); 
    p2 += sizeof(long);
    curmetanamepos_2 = p2 - newdata;



    while(curmetaID_1 && curmetaID_2)
    {            
        p = compress3(min(curmetaID_1,curmetaID_2),p);

        curmetanamepos = p - sw->Index->worddata_buffer;
                /* Store 0 and increase pointer */
        tmp=0L;

        PACKLONG2(tmp,p);
        p+=sizeof(unsigned long);
        if(curmetaID_1 == curmetaID_2)
        {
            /* Both buffers have the same metaID - In this case I have to know
            the number of the filenum of the last hit of the original buffer to adjust the
            filenum counter in the second buffer */
            last_filenum = 0;
            do
            {
                /* Read on all items */
                uncompress_location_values(&p1,&r_flag,&tmpval,&frequency);
                last_filenum += tmpval;  
                if(frequency > POSDATA_STACK)
                    posdata = (int *) emalloc(frequency * sizeof(int));
                else
                    posdata = stack_posdata;
    
                /* Read and discard positions just to advance pointer */
                uncompress_location_positions(&p1,r_flag,frequency,posdata);
                if(posdata!=stack_posdata)
                    efree(posdata);

                if ((p1 - olddata) == sz_olddata)
                {
                    curmetaID_1 = 0;   /* No more metaIDs for olddata */
                    break;   /* End of olddata */
                }

                if ((unsigned long)(p1 - olddata) == nextposmetaname_1)
                {
                    break;
                }
            } while(1);
            memcpy(p,olddata + curmetanamepos_1, p1 - (olddata + curmetanamepos_1));
            p += p1 - (olddata + curmetanamepos_1);
            /* Values for next metaID if exists */
            if(curmetaID_1)
            {
                curmetaID_1 = uncompress2(&p1);  /* Next metaID */
                nextposmetaname_1 = UNPACKLONG2(p1); 
                p1 += sizeof(long);
                curmetanamepos_1 = p1 - olddata;
            }

            /* Now add the new values adjusting with last_filenum just the first
            ** filenum in olddata*/
            /* Read first item */
            uncompress_location_values(&p2,&r_flag,&tmpval,&frequency);
            filenum = tmpval;  /* First filenum in chunk */
            if(frequency > POSDATA_STACK)
                posdata = (int *) emalloc(frequency * sizeof(int));
            else
                posdata = stack_posdata;

            /* Read positions */
            uncompress_location_positions(&p2,r_flag,frequency,posdata);

            compress_location_values(&p,&w_flag,filenum - last_filenum,frequency,posdata);
            compress_location_positions(&p,w_flag,frequency,posdata);

            if(posdata!=stack_posdata)
                efree(posdata);

            /* Copy rest of data */
            memcpy(p,p2,nextposmetaname_2 - (p2 - newdata));
            p += nextposmetaname_2 - (p2 - newdata);
            p2 += nextposmetaname_2 - (p2 - newdata);

            if ((p2 - newdata) == sz_newdata)
            {
                curmetaID_2 = 0;   /* No more metaIDs for newdata */
            }
            /* Values for next metaID if exists */
            if(curmetaID_2)
            {
                curmetaID_2 = uncompress2(&p2);  /* Next metaID */
                nextposmetaname_2 = UNPACKLONG2(p2); 
                p2 += sizeof(long);
                curmetanamepos_2 = p2 - newdata;
            }
        }
        else if (curmetaID_1 < curmetaID_2)
        {
            memcpy(p,p1,nextposmetaname_1 - (p1 - olddata));
            p += nextposmetaname_1 - (p1 - olddata);
            p1 = olddata + nextposmetaname_1;
            if ((p1 - olddata) == sz_olddata)
            {
                curmetaID_1 = 0;   /* No more metaIDs for newdata */
            }
            else
            {
                curmetaID_1 = uncompress2(&p1);  /* Next metaID */
                nextposmetaname_1 = UNPACKLONG2(p1); 
                p1 += sizeof(long);
                curmetanamepos_1 = p1 - olddata;
            }
        }
        else  /* curmetaID_1 > curmetaID_2 */
        {
            memcpy(p,p2,nextposmetaname_2 - (p2 - newdata));
            p += nextposmetaname_2 - (p2 - newdata);
            p2 = newdata + nextposmetaname_2;
            if ((p2 - newdata) == sz_newdata)
            {
                curmetaID_2 = 0;   /* No more metaIDs for newdata */
            }
            else
            {
                curmetaID_2 = uncompress2(&p2);  /* Next metaID */
                nextposmetaname_2 = UNPACKLONG2(p2); 
                p2 += sizeof(long);
                curmetanamepos_2 = p2 - newdata;
            }
        }
        /* Put nextmetaname offset */
        PACKLONG2(p - sw->Index->worddata_buffer, sw->Index->worddata_buffer + curmetanamepos);
       
    } /* while */
  
    /* Add the rest of the data if exists */
    while(curmetaID_1)
    {
        p = compress3(curmetaID_1,p);

        curmetanamepos = p - sw->Index->worddata_buffer;
                /* Store 0 and increase pointer */
        tmp=0L;
        PACKLONG2(tmp,p);
        p += sizeof(unsigned long);

        memcpy(p,p1,nextposmetaname_1 - (p1 - olddata));
        p += nextposmetaname_1 - (p1 - olddata);
        p1 = olddata + nextposmetaname_1;
        if ((p1 - olddata) == sz_olddata)
        {
            curmetaID_1 = 0;   /* No more metaIDs for olddata */
        }
        else
        {
            curmetaID_1 = uncompress2(&p1);  /* Next metaID */
            nextposmetaname_1 = UNPACKLONG2(p1); 
            p1 += sizeof(long);
            curmetanamepos_1 = p1 - olddata;
        }
        PACKLONG2(p - sw->Index->worddata_buffer, sw->Index->worddata_buffer + curmetanamepos);
    }


    while(curmetaID_2)
    {
        p = compress3(curmetaID_2,p);

        curmetanamepos = p - sw->Index->worddata_buffer;
            /* Store 0 and increase pointer */
        tmp=0L;
        PACKLONG2(tmp,p);
        p += sizeof(unsigned long);

        memcpy(p,p2,nextposmetaname_2 - (p2 - newdata));
        p += nextposmetaname_2 - (p2 - newdata);
        p2 = newdata + nextposmetaname_2;
        if ((p2 - newdata) == sz_newdata)
        {
            curmetaID_2 = 0;   /* No more metaIDs for olddata */
        }
        else
        {
            curmetaID_2 = uncompress2(&p2);  /* Next metaID */
            nextposmetaname_2 = UNPACKLONG2(p2); 
            p2+= sizeof(long);
            curmetanamepos_2= p2 - newdata;
        }
    }


    if(newdata != stack_buffer)
        efree(newdata);

    /* Save the new size */
    sw->Index->sz_worddata_buffer = p - sw->Index->worddata_buffer;
}

/* Writes the list of metaNames into the DB index
*/

void    write_MetaNames(SWISH *sw, int id, INDEXDATAHEADER * header, void *DB)
{
    struct metaEntry *entry = NULL;
    int     i,
            sz_buffer,
            len;
    unsigned char *buffer,*s;
    int     fields;

    /* Use new metaType schema - see metanames.h */
    // Format of metaname is
    //   <len><metaName><metaType><Alias><rank_bias>
    //   len, metaType, alias, and rank_bias are compressed numbers
    //   metaName is the ascii name of the metaname
    //
    // The list of metanames is delimited by a 0

    fields = 5;  // len, metaID, metaType, alias, rank_bias

     
    /* Compute buffer size */
    for (sz_buffer = 0 , i = 0; i < header->metaCounter; i++)
    {
        entry = header->metaEntryArray[i];
        len = strlen(entry->metaName);
        sz_buffer += len + fields * MAXINTCOMPSIZE; /* compress can use MAXINTCOMPSIZE bytes in worse case,  */
    }
    
    sz_buffer += MAXINTCOMPSIZE;  /* Add extra MAXINTCOMPSIZE for the number of metanames */

    s = buffer = (unsigned char *) emalloc(sz_buffer);

    s = compress3(header->metaCounter,s);   /* store the number of metanames */

    for (i = 0; i < header->metaCounter; i++)
    {
        entry = header->metaEntryArray[i];
        len = strlen(entry->metaName);
        s = compress3(len, s);
        memcpy(s,entry->metaName,len);
        s += len;
        s = compress3(entry->metaID, s);
        s = compress3(entry->metaType, s);
        s = compress3(entry->alias+1, s);  /* keep zeros away from compress3, I believe */
        s = compress3(entry->rank_bias+RANK_BIAS_RANGE+1, s);
    }
    DB_WriteHeaderData(sw, id,buffer,s-buffer,DB);
    efree(buffer);
}



/* Write a the hashlist of words into the index header file (used by stopwords and buzzwords
*/

int    write_words_to_header(SWISH *sw, int header_ID, struct swline **hash, void *DB)
{
    int     hashval,
            len,
            num_words,
            sz_buffer;
    char   *buffer, *s;
    struct swline *sp = NULL;
        
        /* Let's count the words */

    for (sz_buffer = 0, num_words = 0 , hashval = 0; hashval < HASHSIZE; hashval++)
    {
        sp = hash[hashval];
        while (sp != NULL)
        {
            num_words++;
            sz_buffer += MAXINTCOMPSIZE + strlen(sp->line);
            sp = sp->next;
        }
    }

    if(num_words)
    {
        sz_buffer += MAXINTCOMPSIZE;  /* Add MAXINTCOMPSIZE for the number of words */

        s = buffer = (char *)emalloc(sz_buffer);

        s = (char *)compress3(num_words, (unsigned char *)s);

        for (hashval = 0; hashval < HASHSIZE; hashval++)
        {
            sp = hash[hashval];
            while (sp != NULL)
            {
                len = strlen(sp->line);
                s = (char *)compress3(len,(unsigned char *)s);
                memcpy(s,sp->line,len);
                s +=len;
                sp = sp->next;
            }
        }
        DB_WriteHeaderData(sw, header_ID, (unsigned char *)buffer, s - buffer, DB);
        efree(buffer);
}
return 0;
}



int write_integer_table_to_header(SWISH *sw, int id, int table[], int table_size, void *DB)
{
    int     i,
            tmp;
    char   *s;
    char   *buffer;
    
    s = buffer = (char *) emalloc((table_size + 1) * MAXINTCOMPSIZE);

    s = (char *)compress3(table_size,(unsigned char *)s);   /* Put the number of elements */
    for (i = 0; i < table_size; i++)
    {
        tmp = table[i] + 1;
        s = (char *)compress3(tmp, (unsigned char *)s); /* Put all the elements */
    }

    DB_WriteHeaderData(sw, id, (unsigned char *)buffer, s-buffer, DB);

    efree(buffer);
    return 0;
}








/* General read DB routines - Common to all DB */

/* Reads the file offset table in the index file.
*/

/* Reads and prints the header of an index file.
** Also reads the information in the header (wordchars, beginchars, etc)
*/

// $$$ to be rewritten as function = smaller code (rasc)

#define parse_int_from_buffer(num,s) (num) = UNPACKLONG2((s))
#define parse_int2_from_buffer(num1,num2,s) (num1) = UNPACKLONG2((s));(num2) = UNPACKLONG2((s+sizeof(long)))


void    read_header(SWISH *sw, INDEXDATAHEADER *header, void *DB)
{
    int     id,
            len;
    unsigned long    tmp, tmp1, tmp2;
    unsigned char   *buffer;

    DB_InitReadHeader(sw, DB);

    DB_ReadHeaderData(sw, &id,&buffer,&len,DB);

    while (id)
    {
        switch (id)
        {
        case INDEXHEADER_ID:
        case INDEXVERSION_ID:
        case MERGED_ID:
        case DOCPROPENHEADER_ID:
            break;
        case WORDCHARSHEADER_ID:
            header->wordchars = SafeStrCopy(header->wordchars, (char *)buffer, &header->lenwordchars);
            sortstring(header->wordchars);
            makelookuptable(header->wordchars, header->wordcharslookuptable);
            break;
        case BEGINCHARSHEADER_ID:
            header->beginchars = SafeStrCopy(header->beginchars, (char *)buffer, &header->lenbeginchars);
            sortstring(header->beginchars);
            makelookuptable(header->beginchars, header->begincharslookuptable);
            break;
        case ENDCHARSHEADER_ID:
            header->endchars = SafeStrCopy(header->endchars, (char *)buffer, &header->lenendchars);
            sortstring(header->endchars);
            makelookuptable(header->endchars, header->endcharslookuptable);
            break;
        case IGNOREFIRSTCHARHEADER_ID:
            header->ignorefirstchar = SafeStrCopy(header->ignorefirstchar, (char *)buffer, &header->lenignorefirstchar);
            sortstring(header->ignorefirstchar);
            makelookuptable(header->ignorefirstchar, header->ignorefirstcharlookuptable);
            break;
        case IGNORELASTCHARHEADER_ID:
            header->ignorelastchar = SafeStrCopy(header->ignorelastchar, (char *)buffer, &header->lenignorelastchar);
            sortstring(header->ignorelastchar);
            makelookuptable(header->ignorelastchar, header->ignorelastcharlookuptable);
            break;

        /* replaced by fuzzy_mode Aug 20, 2002     
        case STEMMINGHEADER_ID:
            parse_int_from_buffer(tmp,buffer);
            header-> = tmp;
            break;
        case SOUNDEXHEADER_ID:
            parse_int_from_buffer(tmp,buffer);
            header->applySoundexRules = tmp;
            break;
        */

        case FUZZYMODEHEADER_ID:
            parse_int_from_buffer(tmp,buffer);
            header->fuzzy_mode = tmp;
            break;
            
        case IGNORETOTALWORDCOUNTWHENRANKING_ID:
            parse_int_from_buffer(tmp,buffer);
            header->ignoreTotalWordCountWhenRanking = tmp;
            break;
        case MINWORDLIMHEADER_ID:
            parse_int_from_buffer(tmp,buffer);
            header->minwordlimit = tmp;
            break;
        case MAXWORDLIMHEADER_ID:
            parse_int_from_buffer(tmp,buffer);
            header->maxwordlimit = tmp;
            break;
        case SAVEDASHEADER_ID:
            header->savedasheader = SafeStrCopy(header->savedasheader, (char *)buffer, &header->lensavedasheader);
            break;
        case NAMEHEADER_ID:
            header->indexn = SafeStrCopy(header->indexn, (char *)buffer, &header->lenindexn);
            break;
        case DESCRIPTIONHEADER_ID:
            header->indexd = SafeStrCopy(header->indexd, (char *)buffer, &header->lenindexd);
            break;
        case POINTERHEADER_ID:
            header->indexp = SafeStrCopy(header->indexp, (char *)buffer, &header->lenindexp);
            break;
        case MAINTAINEDBYHEADER_ID:
            header->indexa = SafeStrCopy(header->indexa, (char *)buffer, &header->lenindexa);
            break;
        case INDEXEDONHEADER_ID:
            header->indexedon = SafeStrCopy(header->indexedon, (char *)buffer, &header->lenindexedon);
            break;
        case COUNTSHEADER_ID:
            parse_int2_from_buffer(tmp1,tmp2,buffer);
            header->totalwords = tmp1;
            header->totalfiles = tmp2;
            break;
/* removed due to patents problems
        case FILEINFOCOMPRESSION_ID:
            ReadHeaderInt(itmp, fp);
            header->applyFileInfoCompression = itmp;
            break;
*/
        case TRANSLATECHARTABLE_ID:
            parse_integer_table_from_buffer(header->translatecharslookuptable, sizeof(header->translatecharslookuptable) / sizeof(int), (char *)buffer);
            break;
        case STOPWORDS_ID:
            parse_stopwords_from_buffer(header, (char *)buffer);
            break;
        case METANAMES_ID:
            parse_MetaNames_from_buffer(header, (char *)buffer);
            break;
        case BUZZWORDS_ID:
            parse_buzzwords_from_buffer(header, (char *)buffer);
            break;

#ifndef USE_BTREE
        case TOTALWORDSPERFILE_ID:
            if ( !header->ignoreTotalWordCountWhenRanking )
            {
                header->TotalWordsPerFile = emalloc( header->totalfiles * sizeof(int) );
                parse_integer_table_from_buffer(header->TotalWordsPerFile, header->totalfiles, (char *)buffer);
            }
            break;
#endif

        default:
            progerr("Severe index error in header");
            break;
        }
        efree(buffer);
        DB_ReadHeaderData(sw, &id,&buffer,&len,DB);
    }
    DB_EndReadHeader(sw, DB);
}

/* Reads the metaNames from the index
*/

void    parse_MetaNames_from_buffer(INDEXDATAHEADER *header, char *buffer)
{
    int     len;
    int     num_metanames;
    int     metaType,
            i,
            alias,
            bias,
            metaID;
    char   *word;
    unsigned char   *s = (unsigned char *)buffer;
    struct metaEntry *m;


    /* First clear out the default metanames */
    freeMetaEntries( header );

    num_metanames = uncompress2(&s);

    for (i = 0; i < num_metanames; i++)
    {
        len = uncompress2(&s);
        word = emalloc(len +1);
        memcpy(word,s,len); s += len;
        word[len] = '\0';
        /* Read metaID */
        metaID = uncompress2(&s);
        /* metaType was saved as metaType+1 */
        metaType = uncompress2(&s);

        alias = uncompress2(&s) - 1;

        bias = uncompress2(&s) - RANK_BIAS_RANGE - 1;


        /* add the meta tag */
        if ( !(m = addNewMetaEntry(header, word, metaType, metaID)))
            progerr("failed to add new meta entry '%s:%d'", word, metaID );

        m->alias = alias;
        m->rank_bias = bias;

        efree(word);
    }
}

/* Reads the stopwords in the index file.
*/

void    parse_stopwords_from_buffer(INDEXDATAHEADER *header, char *buffer)
{
    int     len;
    int        num_words;
    int     i;
    char   *word = NULL;

    unsigned char   *s = (unsigned char *)buffer;

    num_words = uncompress2(&s);
    
    for (i=0; i < num_words ; i++)   
    {
        len = uncompress2(&s);
        word = emalloc(len+1);
        memcpy(word,s,len); s += len;
        word[len] = '\0';
        addStopList(header, word);
        addstophash(header, word);
        efree(word);
    }
}

/* read the buzzwords from the index file */

void    parse_buzzwords_from_buffer(INDEXDATAHEADER *header, char *buffer)
{
    int     len;
    int     num_words;
    int     i;
    char   *word = NULL;

    unsigned char   *s = (unsigned char *)buffer;

    num_words = uncompress2(&s);
    for (i=0; i < num_words ; i++)
    {
        len = uncompress2(&s);
        word = emalloc(len+1);
    memcpy(word,s,len); s += len;
        word[len] = '\0';
        addbuzzwordhash(header, word);
    efree(word);
    }
}




void parse_integer_table_from_buffer(int table[], int table_size, char *buffer)
{
    int     tmp,i;
    unsigned char    *s = (unsigned char *)buffer;

    tmp = uncompress2(&s);   /* Jump the number of elements */
    for (i = 0; i < table_size; i++)
    {
        tmp = uncompress2(&s); /* Gut all the elements */
        table[i] = tmp - 1;
    }
}


/* 11/00 Function to read all words starting with a character */
char   *getfilewords(SWISH * sw, int c, IndexFILE * indexf)
{
    int     i,
            j;
    int     wordlen;
    char   *buffer, *resultword;
    int     bufferpos,
            bufferlen;
    unsigned char    word[2];
    long    wordID;

    

    if (!c)
        return "";
    /* Check if already read */
    j = (int) ((unsigned char) c);
    if (indexf->keywords[j])
        return (indexf->keywords[j]);

    DB_InitReadWords(sw, indexf->DB);

    word[0]=(unsigned char)c;
    word[1]='\0';

    DB_ReadFirstWordInvertedIndex(sw, (char *)word, &resultword, &wordID, indexf->DB);
    i = (int) ((unsigned char) c);
    if (!wordID)
    {
        DB_EndReadWords(sw, indexf->DB);
        sw->lasterror = WORD_NOT_FOUND;
        return "";
    }

    wordlen = strlen(resultword);    
    bufferlen = wordlen + MAXSTRLEN * 10;
    bufferpos = 0;
    buffer = emalloc(bufferlen + 1);
    buffer[0] = '\0';


    memcpy(buffer, resultword, wordlen);
    efree(resultword);
    if (c != (int)((unsigned char) buffer[bufferpos]))
    {
        buffer[bufferpos] = '\0';
        indexf->keywords[j] = buffer;
        return (indexf->keywords[j]);
    }

    buffer[bufferpos + wordlen] = '\0';
    bufferpos += wordlen + 1;

    /* Look for occurrences */
    DB_ReadNextWordInvertedIndex(sw, (char *)word, &resultword, &wordID, indexf->DB);
    while (wordID)
    {
        wordlen = strlen(resultword);
        if ((bufferpos + wordlen + 1 + 1) > bufferlen)
        {
            bufferlen += MAXSTRLEN + wordlen + 1 + 1;
            buffer = (char *) erealloc(buffer, bufferlen + 1);
        }
        memcpy(buffer + bufferpos, resultword, wordlen);
        efree(resultword);
        if (c != (int)((unsigned char)buffer[bufferpos]))
        {
            buffer[bufferpos] = '\0';
            break;
        }
        
        buffer[bufferpos + wordlen] = '\0';
        bufferpos += wordlen + 1;
        DB_ReadNextWordInvertedIndex(sw, (char *)word, &resultword, &wordID, indexf->DB);
    }
    buffer[bufferpos] = '\0';
    indexf->keywords[j] = buffer;
    return (indexf->keywords[j]);
}

void setTotalWordsPerFile(SWISH *sw, IndexFILE *indexf, int idx,int wordcount)
{
INDEXDATAHEADER *header = &indexf->header;
#ifdef USE_BTREE
        DB_WriteTotalWordsPerFile(sw, idx, wordcount, indexf->DB);

#else

        if ( !header->TotalWordsPerFile || idx >= header->TotalWordsPerFileMax )
        {
            header->TotalWordsPerFileMax += 20000;  /* random guess -- could be a config setting */
            if(! header->TotalWordsPerFile)
               header->TotalWordsPerFile = emalloc( header->TotalWordsPerFileMax * sizeof(int) );
            else
               header->TotalWordsPerFile = erealloc( header->TotalWordsPerFile, header->TotalWordsPerFileMax * sizeof(int) );
        }

        header->TotalWordsPerFile[idx] = wordcount;
#endif
}


void getTotalWordsPerFile(SWISH *sw, IndexFILE *indexf, int idx,int *wordcount)
{
#ifdef USE_BTREE
        DB_ReadTotalWordsPerFile(sw, idx, wordcount, indexf->DB);
#else
INDEXDATAHEADER *header = &indexf->header;
        *wordcount = header->TotalWordsPerFile[idx];
#endif
}


/*------------------------------------------------------*/
/*---------- General entry point of DB module ----------*/

void   *DB_Create (SWISH *sw, char *dbname)
{
   return sw->Db->DB_Create(sw, dbname);
}

void   *DB_Open (SWISH *sw, char *dbname, int mode)
{
   return sw->Db->DB_Open(sw, dbname,mode);
}

void    DB_Close(SWISH *sw, void *DB)
{
   sw->Db->DB_Close(DB);
}


void    DB_Remove(SWISH *sw, void *DB)
{
   sw->Db->DB_Remove(DB);
}

int     DB_InitWriteHeader(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitWriteHeader(DB);
}

int     DB_WriteHeaderData(SWISH *sw, int id, unsigned char *s, int len, void *DB)
{
   return sw->Db->DB_WriteHeaderData(id, s,len,DB);
}

int     DB_EndWriteHeader(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndWriteHeader(DB);
}


int     DB_InitReadHeader(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitReadHeader(DB);
}

int     DB_ReadHeaderData(SWISH *sw, int *id, unsigned char **s, int *len, void *DB)
{
   return sw->Db->DB_ReadHeaderData(id, s, len, DB);
}

int     DB_EndReadHeader(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndReadHeader(DB);
}


int     DB_InitWriteWords(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitWriteWords(DB);
}

long    DB_GetWordID(SWISH *sw, void *DB)
{
   return sw->Db->DB_GetWordID(DB);
}

int     DB_WriteWord(SWISH *sw, char *word, long wordID, void *DB)
{
   return sw->Db->DB_WriteWord(word, wordID, DB);
}

#ifdef USE_BTREE
int     DB_UpdateWordID(SWISH *sw, char *word, long wordID, void *DB)
{
   return sw->Db->DB_UpdateWordID(word, wordID, DB);
}

int     DB_DeleteWordData(SWISH *sw, long wordID, void *DB)
{
   return sw->Db->DB_DeleteWordData(wordID, DB);
}

#endif

int     DB_WriteWordHash(SWISH *sw, char *word, long wordID, void *DB)
{
   return sw->Db->DB_WriteWordHash(word, wordID, DB);
}

long    DB_WriteWordData(SWISH *sw, long wordID, unsigned char *worddata, int lendata, void *DB)
{
   return sw->Db->DB_WriteWordData(wordID, worddata, lendata, DB);
}

int     DB_EndWriteWords(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndWriteWords(DB);
}


int     DB_InitReadWords(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitReadWords(DB);
}

int     DB_ReadWordHash(SWISH *sw, char *word, long *wordID, void *DB)
{
   return sw->Db->DB_ReadWordHash(word, wordID, DB);
}

int     DB_ReadFirstWordInvertedIndex(SWISH *sw, char *word, char **resultword, long *wordID, void *DB)
{
   return sw->Db->DB_ReadFirstWordInvertedIndex(word, resultword, wordID, DB);
}

int     DB_ReadNextWordInvertedIndex(SWISH *sw, char *word, char **resultword, long *wordID, void *DB)
{
   return sw->Db->DB_ReadNextWordInvertedIndex(word, resultword, wordID, DB);
}

long    DB_ReadWordData(SWISH *sw, long wordID, unsigned char **worddata, int *lendata, void *DB)
{
   return sw->Db->DB_ReadWordData(wordID, worddata, lendata, DB);
}

int     DB_EndReadWords(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndReadWords(DB);
}



int     DB_InitWriteFiles(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitWriteFiles(DB);
}

int     DB_WriteFile(SWISH *sw, int filenum, unsigned char *filedata,int sz_filedata, void *DB)
{
   return sw->Db->DB_WriteFile(filenum, filedata, sz_filedata, DB);
}

int     DB_EndWriteFiles(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndWriteFiles(DB);
}


int     DB_InitReadFiles(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitReadFiles(DB);
}

int     DB_ReadFile(SWISH *sw, int filenum, unsigned char **filedata,int *sz_filedata, void *DB)
{
   return sw->Db->DB_ReadFile(filenum, filedata,sz_filedata, DB);
}

int     DB_EndReadFiles(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndReadFiles(DB);
}

#ifdef USE_BTREE
int     DB_InitWriteSortedIndex(SWISH *sw, void *DB, int n_props)
{
   return sw->Db->DB_InitWriteSortedIndex(DB, n_props);
}
#else
int     DB_InitWriteSortedIndex(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitWriteSortedIndex(DB);
}
#endif
int     DB_WriteSortedIndex(SWISH *sw, int propID, unsigned char *data, int sz_data,void *DB)
{
   return sw->Db->DB_WriteSortedIndex(propID, data, sz_data,DB);
}

int     DB_EndWriteSortedIndex(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndWriteSortedIndex(DB);
}

 
int     DB_InitReadSortedIndex(SWISH *sw, void *DB)
{
   return sw->Db->DB_InitReadSortedIndex(DB);
}

int     DB_ReadSortedIndex(SWISH *sw, int propID, unsigned char **data, int *sz_data,void *DB)
{
   return sw->Db->DB_ReadSortedIndex(propID, data, sz_data,DB);
}

int     DB_ReadSortedData(SWISH *sw, int *data,int index, int *value, void *DB)
{
   return sw->Db->DB_ReadSortedData(data,index,value,DB);
}

int     DB_EndReadSortedIndex(SWISH *sw, void *DB)
{
   return sw->Db->DB_EndReadSortedIndex(DB);
}


void DB_WriteProperty( SWISH *sw, IndexFILE *indexf, FileRec *fi, int propID, char *buffer, int buf_len, int uncompressed_len, void *db)
{
    sw->Db->DB_WriteProperty( indexf, fi, propID, buffer, buf_len, uncompressed_len, db );
}

void    DB_WritePropPositions(SWISH *sw, IndexFILE *indexf, FileRec *fi, void *db)
{
    sw->Db->DB_WritePropPositions( indexf, fi, db);
}

void    DB_ReadPropPositions(SWISH *sw, IndexFILE *indexf, FileRec *fi, void *db)
{
    sw->Db->DB_ReadPropPositions( indexf, fi, db);
}


char *DB_ReadProperty(SWISH *sw, IndexFILE *indexf, FileRec *fi, int propID, int *buf_len, int *uncompressed_len, void *db)
{
    return sw->Db->DB_ReadProperty( indexf, fi, propID, buf_len, uncompressed_len, db );
}


void    DB_Reopen_PropertiesForRead(SWISH *sw, void *DB )
{
    sw->Db->DB_Reopen_PropertiesForRead(DB);
}


#ifdef USE_BTREE

int    DB_WriteTotalWordsPerFile(SWISH *sw, int idx, int wordcount, void *DB)
{
    return sw->Db->DB_WriteTotalWordsPerFile(sw, idx, wordcount, DB);
}


int       DB_ReadTotalWordsPerFile(SWISH *sw, int index, int *value, void *DB)
{
    return sw->Db->DB_ReadTotalWordsPerFile(sw, index, value, DB);
}

#endif


