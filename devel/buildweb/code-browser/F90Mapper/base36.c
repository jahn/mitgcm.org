char *base36(n)
     /* Returns a character string representing an integer in base 36 */
int n;
{
    char *base = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int  i, l;
    char buffer,num[500];
    static char rev[500]; 
    i = strlen(base);
    l=0;
    while ( n > i-1 ) {
     num[l]=base[n-i*(n/i)];
     ++l;
     n = n/i;
    }
    num[l]=base[n];
    rev[l+1]='\0';
    for (i=0;l>=0;--l){
     rev[i]=num[l];
     ++i;
    }
    return(rev);
}

