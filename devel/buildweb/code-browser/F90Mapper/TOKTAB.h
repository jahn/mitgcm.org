struct symTab  { char *symName;
                 char *ucName;
                 int  useCount;
                 struct symTab *next; };
typedef struct symTab  symTab;
symTab *symTabHead  = NULL;   /* Table of variable names         */
symTab *procTabHead = NULL;   /* Table of procedure names        */
symTab *extTabHead  = NULL;   /* Table of current external names */
int symbolCount = 0;
int procCount   = 0;
