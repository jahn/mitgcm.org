BEGIN{MATCH_ROUTINE=0;
i=0;nVals=0;
while ( getline < "varkey" ) {
 var[i]=$1;
 val[i]=$2;
 ++i;
 nVals=i-1;
}
print "<HTML>"
print "<HEAD>"
print "<TITLE>MITgcm Call Tree</TITLE>"
print "</HEAD>"
print "<BODY leftmargin=0 topmargin=0 marginheight=0 marginwidth=0 bgcolor=#FFFFFF>"
while ( getline < "callTree.header" ) {
 print $0;
}
print "<PRE>"
}
/\|-[A-Z,0-9,_]*.*/{
  match($0,/\|-[A-Z0-9_]*/);
  theString=substr($0,RSTART+2,RLENGTH-2);
  rsString=RSTART; rlString=RLENGTH;
  nMatch=-1;
  for ( i=0;i<nVals;++i) {
   if ( match(var[i],theString) == 1 && length(var[i]) == length(theString) && length(theString) != 0 ) {
    nMatch=i;
    break;
   }
  }
  if ( nMatch >= 0 ) {
   print substr($0,0,rsString)"<A HREF=vdb/names/"val[nMatch]".htm target=codeBrowserWindow>" theString "</A>" substr($0,rsString+rlString,length());
   MATCH_ROUTINE=1;
  }
}
{
 if (MATCH_ROUTINE ==0 ){ print};
 MATCH_ROUTINE=0;
}
END{
print "</PRE>"
print "</BODY>"
print "</HTML>"
}
