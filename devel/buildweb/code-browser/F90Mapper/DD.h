/* $Id: DD.h,v 1.1 1997/03/22 20:02:35 cnh Exp $*/

/* =================== Data Dictionary dd routines ================ */

/* Return codes */
#define  ddNONAME    0       /* No name in record passed to routine */
#define  ddALLOCERR -1       /* Memory allocation failure           */

/* Structure of data dictionary record */
struct ddRecord {
  char *name;                  /* Variable name      */
  int     id;                  /* Numeric identifier */
  char  *key;                  /* Character key ( used to reduce HTML file
                                  size.  */
  char *hrefEntry;
  char *textEntry;
  char *footNotesEntry;
  char *unitsEntry;
  int  active;
  int  isInNameList;           /* Flag that name is used in a */
                               /* namelist.                   */
  int  isInIfdef;              /* Flag that name is used in a */
                               /* ifdef.                      */
  int  isProcName;             /* Flag that name is used a    */
                               /* procedure name.             */
};
typedef struct ddRecord ddRecord;

ddRecord *ddAdd(/* ddRecord * */);
    /* Add record at end of dictionary.                     */
    /* Returns ddRecord.id for new entry. <=0 means failed. */
    /* ddRecord needs to have char *name set on entry. Other*/
    /* entries are optional. NULL or 0 is used to indicate  */
    /* unspecified parameter. int id is set by ddInsert.    */
    /* int id value on entry is ignored.                    */
    /* Record is inserted at current record and current     */
    /* record is moved forward one.                         */

int ddSort(/*  */);
    /* Sort dictionary.                                     */
    /* Current record is set to first record.               */

int ddGetCurrent(/* ddRecord * */);
    /* Returns current record. 0 indicates end of table.    */
    /* Moves current record pointer forward one.            */

ddRecord *ddFind(/* *ddRecord */);
         /* Find a record that matches ddRecord->name       */
 
int ddDestroy(/*  */);
    /* Free up dictionary.                                  */
