/*
$Id: parse_conffile.h,v 1.12 2002/08/22 22:58:39 whmoseley Exp $
**
**
*/

void grabCmdOptions(StringList *sl, int start, struct swline **listOfWords);
void getdefaults(SWISH *sw, char *conffile, int *hasdir, int *hasindex, int hasverbose);
int getYesNoOrAbort (StringList *sl, int n, int islast);
int	strtoDocType( char * s );

void free_Extracted_Path( SWISH *sw );
void free_regex_list( regex_list **reg_list );
void freeSwishConfigOptions( SWISH *sw );




