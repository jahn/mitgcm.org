/*
$Id: swish_words.c,v 1.15 2002/08/22 22:58:39 whmoseley Exp $
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
** 2001-05-23  moseley  created - replaced parser in search.c
**
** 2001-12-11  moseley, updated to deal with swish operators inside of phrases
**                      Still broken with regard to double-quotes inside of phrases
**                      Very unlikely someone would want to search for a single double quote
**                      within a phrase.  It currently works if the double-quotes doesn't have
**                      white space around.  Really should tag the words as being operators, or
**                      or "swish words", or let the backslash stay in the query until searching.
**
*/

#include "swish.h"
#include "mem.h"
#include "string.h"
#include "search.h"
#include "index.h"
#include "file.h"
#include "list.h"
#include "hash.h"
#include "stemmer.h"
#include "soundex.h"
#include "double_metaphone.h"
#include "error.h"
#include "metanames.h"
#include "config.h"         // for _AND_WORD...
#include "search_alt.h"     // for AND_WORD... humm maybe needs better organization

struct MOD_Swish_Words
{
    char   *word;
    int     lenword;
};

/* 
  -- init structures for this module
*/

void initModule_Swish_Words (SWISH  *sw)
{
    struct MOD_Swish_Words *self;

    self = (struct MOD_Swish_Words *) emalloc(sizeof(struct MOD_Swish_Words));
    sw->SwishWords = self;

    /* initialize buffers used by indexstring */
    self->word = (char *) emalloc((self->lenword = MAXWORDLEN) + 1);

    return;
}

void freeModule_Swish_Words (SWISH *sw)
{
  struct MOD_Swish_Words *self = sw->SwishWords;

  efree( self->word );
  efree ( self );
  sw->SwishWords = NULL;

  return;
}





/* Returns true if the character is a search operator */
/* this could be a macro, but gcc is probably smart enough */

static int isSearchOperatorChar( int c, int phrase_delimiter, int inphrase )
{
    return inphrase
        ? ( '*' == c || c == phrase_delimiter )
        : ( '(' == c || ')' == c || '=' == c || '*' == c || c == phrase_delimiter );
}


/* This simply tokenizes by whitespace and by the special characters "()=" */
/* If within a phrase, then just splits by whitespace */

/* Funny how argv was joined into a string just to be split again... */

static int next_token( char **buf, char **word, int *lenword, int phrase_delimiter, int inphrase )
{
    int     i;
    int     backslash;

    **word = '\0';

    /* skip any leading whitespace */
    while ( **buf && isspace( (unsigned char) **buf) )
        (*buf)++;


    /* extract out word */
    i = 0;
    backslash = 0;
    
    while ( **buf && !isspace( (unsigned char) **buf) )
    {

        // This should be looking at swish words, not raw input
        //if ( i > max_size + 4 )   /* leave a little room for operators */
        //    progerr( "Search word exceeded maxwordlimit setting." ); 

    
        /* reallocate buffer, if needed -- only if maxwordlimit was set larger than MAXWORDLEN (1000) */
        if ( i == *lenword )
        {
            *lenword *= 2;
            *word = erealloc(*word, *lenword + 1);
        }


        /* backslash says take next char as-is */
        /* note that you cannot backslash whitespace */
        if ( '\\' == **buf && ! backslash++ )
        {
            (*buf)++;
            continue;
        }


        if ( backslash || !isSearchOperatorChar( (unsigned char) **buf, phrase_delimiter, inphrase ) )
        {
            backslash = 0;
            
            (*word)[i++] = **buf;  /* can't this be done in one line? */
            (*buf)++;
        }

        else  /* this is a search operator char */
        {
            if ( **word )  /* break if characters already found - end of this token */
                break;

            (*word)[i++] = **buf;  /* save the search operator char as it's own token, and end. */
            (*buf)++;
            break;
        }

    }


    /* flag if we found a token */
    if ( i )
    {
        (*word)[i] = '\0';
        return 1;
    }

    return 0;
}


static int next_swish_word(INDEXDATAHEADER *header, char **buf, char **word, int *lenword )
{
    int     i;

    /* skip non-wordchars */
    while ( **buf && !header->wordcharslookuptable[tolower((unsigned char)(**buf))] )
        (*buf)++;

    i = 0;
    while ( **buf && header->wordcharslookuptable[tolower((unsigned char)(**buf))] )
    {
        /* reallocate buffer, if needed */
        if ( i + 1 == *lenword )
        {
            *lenword *= 2;
            *word = erealloc(*word, *lenword + 1);
        }

        (*word)[i++] = **buf;
        (*word)[i] = '\0';
        (*buf)++;
    }


    if ( i )
    {
        stripIgnoreLastChars( header, *word);
        stripIgnoreFirstChars(header, *word);


        return **word ? 1 : 0;
    }

    return 0;
}


/* Convert a word into swish words */

static struct swline *parse_swish_words( SWISH *sw, INDEXDATAHEADER *header, char *word, int max_size )
{
    struct  swline  *swish_words = NULL;
    char   *curpos;
    struct  MOD_Swish_Words *self = sw->SwishWords;



    /* Some initial adjusting of the word */


    TranslateChars(header->translatecharslookuptable, (unsigned char *)word);



    curpos = word;
    while( next_swish_word( header, &curpos, &self->word, &self->lenword ) )
    {
        /* Check Begin & EndCharacters */
        if (!header->begincharslookuptable[(int) ((unsigned char) self->word[0])])
            continue;


        if (!header->endcharslookuptable[(int) ((unsigned char) self->word[strlen(self->word) - 1])])
            continue;


        /* limit by stopwords, min/max length, max number of digits, ... */
        /* ------- processed elsewhere for search ---------
        if (!isokword(sw, self->word, indexf))
            continue;
        - stopwords are processed in search.c because removing them may have side effects
        - maxwordlen is checked when first tokenizing for security reasons
        - limit by vowels, consonants and digits is not needed since search will just fail
        ----------- */
        if ( strlen( self->word ) > max_size )
        {
            sw->lasterror = SEARCH_WORD_TOO_BIG;
            return NULL;
        }

        if (!*self->word)
            continue;

        switch ( header->fuzzy_mode )
        {
            case FUZZY_NONE:
                swish_words = (struct swline *) addswline( swish_words, self->word );
                break;

            case FUZZY_STEMMING:
                Stem(&self->word, &self->lenword);
                if ( *self->word ) // should not happen
                    swish_words = (struct swline *) addswline( swish_words, self->word );
                break;

                
            case FUZZY_SOUNDEX:
                soundex(self->word);
                if ( *self->word )
                    swish_words = (struct swline *) addswline( swish_words, self->word );
                break;

            case FUZZY_METAPHONE:
            case FUZZY_DOUBLE_METAPHONE:
                {
                    char *codes[2];
                    DoubleMetaphone(self->word, codes);

                    if ( !(*codes[0]) )
                    {
                        efree( codes[0] );
                        efree( codes[1] );
                        swish_words = (struct swline *) addswline( swish_words, self->word );
                        break;
                    }


                    /* check if just METAPHONE or only one word returned (e.g. they are the same) */
                
                    if ( header->fuzzy_mode == FUZZY_METAPHONE || !(*codes[1]) || !strcmp(codes[0], codes[1]) )
                    {
                        swish_words = (struct swline *) addswline( swish_words, codes[0] );
                    }
                    else
                    {
                        /* yuck! */
                        swish_words = (struct swline *) addswline( swish_words, "(" );
                        swish_words = (struct swline *) addswline( swish_words, codes[0] );
                        swish_words = (struct swline *) addswline( swish_words, "or" );
                        swish_words = (struct swline *) addswline( swish_words, codes[1] );
                        swish_words = (struct swline *) addswline( swish_words, ")" );
                    }

                    efree( codes[0] );
                    efree( codes[1] );
                }
                break;
                

            default:
                progerr("Invalid FuzzyMode '%d'", (int)header->fuzzy_mode );
        }
    }

    return swish_words;


}

/* This is really dumb.  swline needs a ->prev entry, really search needs its own linked list */
/* Replaces a given node with another node (or nodes) */

static void  replace_swline( struct swline **original, struct swline *entry, struct swline *new_words )
{
    struct swline *temp;


    temp = *original;


    /* check for case of first one */
    if ( temp == entry )
    {
        if ( new_words )
        {
            new_words->nodep->next = temp->next;
            new_words->nodep = temp->nodep;
            *original = new_words;
        } 
        else /* just delete first node */
        {
            if ( entry->next )
                entry->next->nodep = entry->nodep; /* point next one to last one */
            *original = entry->next;
        }
    }



    else /* not first node */
    {
        /* search for the preceeding node */
        for ( temp = *original; temp && temp->next != entry; temp = temp->next );

        if ( !temp )
            progerr("Fatal Error: Failed to find insert point in replace_swline");

        if ( new_words )
        {
            /* set the previous record to point to the start of the new entry (or entries) */
            temp->next = new_words;

            /* set the end of the new string to point to the next entry */
            new_words->nodep->next = entry->next;
        }
        else /* delete the entry */
            temp->next = temp->next->next;
    }
    

    /* now free the removed item */
    efree( entry->line );
    efree( entry );


}


static int checkbuzzword(INDEXDATAHEADER *header, char *word )
{
    if ( !header->buzzwords_used_flag )
        return 0;

        
    /* only strip when buzzwords are being used since stripped again as a "swish word" */
    stripIgnoreLastChars( header, word );
    stripIgnoreFirstChars( header, word );
    
    if ( !*word ) /* stripped clean? */
        return 0;


    return isbuzzword( header, word);
}

/* I hope this doesn't live too long */

static void fudge_wildcard( struct swline **original, struct swline *entry )
{
    char    *tmp;
    struct swline *wild_card;

    wild_card = entry->next;        

    /* reallocate a string */
    tmp = entry->line;
    entry->line = emalloc( strlen( entry->line ) + 2 );
    strcpy( entry->line, tmp);
    efree( tmp );
    strcat( entry->line, "*");

    efree( wild_card->line );


    /* removing last entry - set pointer to new end */
    if ( (*original)->nodep == wild_card )
        (*original)->nodep = entry;

    /* and point next to the one after next */
    entry->next = wild_card->next;


    efree( wild_card );
}

    
    
/******************** Public Functions *********************************/

char *isBooleanOperatorWord( char * word )
{
    /* don't need strcasecmp here, since word should alrady be lowercase -- need to check alt-search first */
    if (!strcasecmp( word, _AND_WORD))
        return AND_WORD;

    if (!strcasecmp( word, _OR_WORD))
        return OR_WORD;

    if (!strcasecmp( word, _NOT_WORD))
        return NOT_WORD;

    return (char *)NULL;
}



struct swline *tokenize_query_string( SWISH *sw, char *words, INDEXDATAHEADER *header )
{
    char   *curpos;               /* current position in the words string */
    struct  swline *tokens = NULL;
    struct  swline *temp;
    struct  swline *swish_words;
    struct  swline *next_node;
    struct  MOD_Swish_Words *self = sw->SwishWords;
    struct  MOD_Search *srch = sw->Search;
    unsigned char PhraseDelimiter;
    int     max_size;
    int     inphrase = 0;


    /* Probably won't get to this point */
    if ( !words || !*words )
    {
        sw->lasterror = NO_WORDS_IN_SEARCH;
        return NULL;
    }


    PhraseDelimiter = (unsigned char) srch->PhraseDelimiter;
    max_size        = header->maxwordlimit;

    curpos = words;  

    /* split into words by whitespace and by the swish operator characters */
    
    while ( next_token( &curpos, &self->word, &self->lenword, PhraseDelimiter, inphrase ) )
    {
        tokens = (struct swline *) addswline( tokens, self->word );


        if ( self->word[0] == PhraseDelimiter && !self->word[1] )
            inphrase = !inphrase;
    }


    /* no search words found */
    if ( !tokens )
        return tokens;


    inphrase = 0;

    temp = tokens;
    while ( temp )
    {

        /* do look-ahead processing first -- metanames */

        if ( !inphrase && isMetaNameOpNext(temp->next) )
        {

            if( !getMetaNameByName( header, temp->line ) )
            {
                set_progerr( UNKNOWN_METANAME, sw, "'%s'", temp->line );
                freeswline( tokens );
                return NULL;
            }


            /* this might be an option with XML */
            strtolower( temp->line );

            temp = temp->next;
            continue;
        }
                

        /* skip operators */
        if ( strlen( temp->line ) == 1 && isSearchOperatorChar( (unsigned char) temp->line[0], PhraseDelimiter, inphrase ) )
        {

            if ( temp->line[0] == PhraseDelimiter && !temp->line[1] )
                inphrase = !inphrase;

            temp = temp->next;
            continue;
        }

        /* this might be an option if case sensitive searches are used */
        strtolower( temp->line );


        /* check Boolean operators -- currently doesn't change it (search.c does) */
        if ( !inphrase )
        {
            char *operator, *nextoperator;

            if ( (operator = isBooleanOperatorWord( temp->line )) )
            {
                /* replace the common "and not" with simply not" */
                /* probably not the best place to do this level of processing */
                /* since should also check for things like "and this" and "and and and not this" */
                /* should probably be moved to end and recursively check for these (to catch "and and not") */
                if (
                    temp->next
                    && ( strcmp( operator, AND_WORD ) == 0)
                    && ( (nextoperator = isBooleanOperatorWord( temp->next->line)))
                    && ( strcmp( nextoperator, NOT_WORD ) == 0)
                ) {
                    struct swline *andword = temp; /* save position */

                    temp = temp->next;  /* now point to "not" word */
                    replace_swline( &tokens, andword, (struct swline *)NULL ); /* cut it out */
                    continue;
                }

                temp = temp->next;
                continue;
            }
        }
    

        /* buzzwords */
        if ( checkbuzzword( header, temp->line ) )
        {
            temp = temp->next;
            continue;
        }



        /* query words left.  Turn into "swish_words" */
        swish_words = NULL;
        swish_words = parse_swish_words( sw, header, temp->line, max_size);

        if ( sw->lasterror )
            return NULL;

        
        next_node = temp->next;

        /* move into list.c at some point */
        replace_swline( &tokens, temp, swish_words );
        temp = next_node;
        
    }

    /* fudge wild cards back onto preceeding word */
    for ( temp = tokens ; temp; temp = temp->next )
        if ( temp->next && strcmp( temp->next->line, "*") == 0 )
            fudge_wildcard( &tokens, temp );


    return tokens;
}

