#! /usr/bin/env bash

# $Header:  $

#BLDDIR='/u/u0/httpd/html/build_manual'
#cd $BLDDIR
#retval=$?
#if test "x$retval" != x0 ; then
#  echo "unable to cd to '$BLDDIR' => exit"
#  exit
#fi
echo -n "-- Start '"`basename $0`"' at : " ; date

export CVSROOT=/u/gcmpack
#OUTDIR='/u/u0/httpd/html/r2_manual'
#- note: "mv" to relative path below is much faster than using full path above
OUTDIR='../r2_manual'

echo
echo "  Removing previous directories..."
test -e old  &&  rm -rf old
mkdir old
mv MITgcm manual mitgcm.org old
rm -rf old &

if test -e scratch/dev_docs ; then
    mv scratch/dev_docs "scratch/dev_docs_"`date +%Y%m%d`"_"`date +%H%M`
fi

echo
echo "-- Download from CVS :"
rm -f dwn.log
echo -n ' MITgcm ... '
cvs -q co -P MITgcm > dwn.log
echo "=====================================" >> dwn.log
echo -n ', manual ... ' ; 
cvs -q co -P manual >> dwn.log
echo "=====================================" >> dwn.log
echo -n ', mitgcm.org ...'
cvs -q co -P mitgcm.org >> dwn.log
echo -n " : done " ; date

(
    rm -f build.log
    cd mitgcm.org/devel/buildweb
    make All | tee ../../../build.log
)
echo -n '-- Finish building manual at : ' ; date
if test -e scratch/dev_docs ; then
    newbld="dev_docs_"`date +%Y%m%d`"_"`date +%H%M`
    mv scratch/dev_docs $OUTDIR/$newbld
fi
#exit

(
  cd $OUTDIR
  test -e latest  &&  rm -f latest
  ln -s `ls -t | head -1` latest
# n=$(( `ls -d dev_docs* | wc -l` - 7 ))
  n=$(( `ls dev_docs*/index.html | wc -l` - 7 ))
  if test $n -gt 0 ; then
#   ls -td dev_docs* | tail -"$n" | xargs rm -rf
    ls -t dev_docs*/index.html | sed 's/\/index.html//' | tail -"$n" | xargs rm -rf
  fi
)

echo -n '-- Done at : ' ; date
