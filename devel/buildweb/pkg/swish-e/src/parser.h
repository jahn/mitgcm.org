/*
$Id: parser.h,v 1.5 2001/10/11 22:21:14 whmoseley Exp $ 
**
**
** The prototypes
*/

int parse_HTML(SWISH *sw, FileProp *fprop, FileRec *fi, char *buffer);
int parse_XML(SWISH *sw, FileProp *fprop, FileRec *fi, char *buffer);
int parse_TXT(SWISH * sw, FileProp * fprop, FileRec *fi, char *buffer);


