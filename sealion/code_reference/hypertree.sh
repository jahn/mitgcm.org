#!/bin/csh -f
#
#  Make a call tree clickable
#

# Log file where keys for names will be found
set LFILE = ( f90log )

# Call tree file
set CTFILE  = ( xx )
set OUTFILE = (  callTree.html )

# Pull out routine names
set rl=`grep '[ |-]*-[A-Z_0-9][A-Z_0-9]*[ (]*.*' $CTFILE | sed s/'[^:]*-\([A-Z_0-9]*\).*/\1/'`

# Find key for each name in log file
echo $rl
cp ${CTFILE} ${CTFILE}.alt1
foreach n ( $rl )
 set key = ( `grep '[ ]'$n'[ ]' $LFILE | grep '^New' | awk '{print $6}'` )
 set sub = `echo "HREF=vdb/names/"$key".htm>"$n"<\/A>"`
#echo \"$n\"
#echo \"$key\"
 cat ${CTFILE}.alt1 | sed s'%-'$n'\([ (]\)%-<A '$sub'\1%' > ${CTFILE}.alt2
 cp  ${CTFILE}.alt2 ${CTFILE}.alt1
end
cp ${CTFILE}.alt1 $OUTFILE
