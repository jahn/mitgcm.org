#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "GLOBALS.h"

int makeOutputDirectories(dName) 
    /* Create a directory for output if it soed not already exist */
char *dName;
{
  int rc;
  char dNamBuf[MAXPATHNAM];

  if ( strlen(dName) > MAXPATHNAM - strlen(VARSUF)-2 ){
   return(-1);
  }
  rc = mkdir(dName, 0755);
  if ( rc != 0 ){ if ( errno != EEXIST ) return(-1); }
  rootDir = strdup(dName);
  sprintf(dNamBuf,"%s/%s",dName,VARSUF);
  rc = mkdir(dNamBuf, 0755);
  if ( rc != 0 ){ if ( errno != EEXIST ) return(-1); }
  varDir = strdup(dNamBuf);
  sprintf(dNamBuf,"%s/%s",dName,SRCSUF);
  rc = mkdir(dNamBuf, 0755);
  if ( rc != 0 ){ if ( errno != EEXIST ) return(-1); }
  srcDir = strdup(dNamBuf);

  return(0);
}
