#!/bin/csh -f
#
# Apply code parsing programs to a set of source files
#
set SRCDIR     = ./work
set CURDIR     = `pwd`

echo 'Parsing code'
#eh3  echo 'Code revision requested is '$1

\rm -f $SRCDIR

# A simple example
#mkdir -p $SRCDIR
#cd  $SRCDIR
#cp -pr ${CURDIR}/model .
#cp -pr ${CURDIR}/pkg   .
#cp -pr ${CURDIR}/callTree.F .
#cp -pr ${CURDIR}/callTree.header .

# Applying it to the whole code ( first -d will only work on local 18.24.3 system )
#eh3  Replace CVS checkout with a soft-link
#eh3 cvs -d /u/gcmpack co -d $SRCDIR -P -r $1 MITgcm > checkout.log
if (-d ../../../../../MITgcm) then
  ln -s ../../../../../MITgcm work
else
  echo "ERROR:  please get a copy of the MITgcm code and locate it at:"
  echo "        "`pwd`"../../../../../MITgcm"
endif
#eh3
cp -fpr callTree.F $SRCDIR
cp -fpr callTree.header $SRCDIR
cd $SRCDIR

set fl = `/usr/bin/find . -name '*.F'`
set hl = `/usr/bin/find . -name '*.h'`

# Extract definition information
### echo "o Extracting definition information"
### ( cd ${CURDIR}/../DefinitionParser; make )
### cat $fl $hl | ${CURDIR}/../DefinitionParser/a.out > thedefs

# Generate hyperlinked code and browser files.
echo "o Generating hyperlinked code (this takes a few minutes)"
( cd ${CURDIR}/../F90Mapper; make )
${CURDIR}/../F90Mapper/f90mapper -d thedefs $fl $hl >& mapper.log

#cnh debugging
#cnh exit

# Make the calling tree hyperlinked
# callTree.F was hand-extracted from model/src/the_model_main.F
echo "o Generating hyperlinked call tree"
grep 'New variable' mapper.log | awk '{print $3, $6}' > varkey
awk -f ${CURDIR}/parseCallTree.awk callTree.F > callTree.html

# Create and fill the subdirectories for the MITgcm web pages
echo "o Making buildweb sub-directory entries for skeletons/code_reference/"
\rm -fr buildweb
mkdir -p buildweb/skeleton/code_reference
cp code_reference-rtparm_exp.htm   buildweb/skeleton/code_reference/parameters.html
cp code_reference-sf_exp.htm       buildweb/skeleton/code_reference/sourcefiles.html
cp code_reference-subfunc_exp.htm  buildweb/skeleton/code_reference/subroutine.html
cp code_reference-vi_exp.htm       buildweb/skeleton/code_reference/variabledictionary.html
cp callTree.html                   buildweb/skeleton/code_reference

echo "o Making buildweb sub-directory entries for vdb/"
mv vdb buildweb/vdb

# Tar up buildweb part
( cd  buildweb ; rm vdb/tmp1; rm vdb/tmp2; tar -czvf ../web.tgz . >& /dev/null )
