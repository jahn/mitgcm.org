/* $Id: FD.h,v 1.1 1997/03/22 20:02:35 cnh Exp $*/

/* =================== File directory info. ======================= */

/* Structure of file directory records */
struct fdDRecord {
  char *name;                  /* Directory name     */
};
struct fdFRecord {
  char *name;                  /* Source file name   */
  char *hname;                 /* HTML file name     */
};
typedef struct fdDRecord fdDRecord;
typedef struct fdFRecord fdFRecord;

/* Source and HTML linked lists */
struct fdSTab {
  fdFRecord fileNam;
  struct fdSTab *next;
  struct fdSTab *prev;
};
typedef struct fdSTab fdSTab;

/* Directory linked list */
struct fdDTab {
  fdDRecord dir;
  struct fdDTab *next;
  struct fdDTab *prev;
  fdSTab *sList;
};
typedef struct fdDTab fdDTab;

fdDTab *fdList;
fdDTab *fdListHead;
fdDTab *fdListTail;
