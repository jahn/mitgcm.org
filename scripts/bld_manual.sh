#! /usr/bin/env bash

# $Header: /u/gcmpack/mitgcm.org/scripts/bld_manual.sh,v 1.1 2008/03/03 01:47:26 jmc Exp $

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
    echo " mv scratch/dev_docs $OUTDIR/$newbld"
    mv scratch/dev_docs $OUTDIR/$newbld
fi
#exit

(
  cd $OUTDIR
  echo -n "-- Install latest in dir: "`pwd`
  test -e latest  &&  rm -f latest
  echo -n " ln -s `ls -td dev_docs* | head -1` latest"
  ln -s `ls -td dev_docs* | head -1` latest
  n=$(( `ls dev_docs*/index.html | wc -l` - 7 ))
  if test $n -gt 0 ; then
    echo -n ' remove dir: ' 
    ls -t dev_docs*/index.html | sed 's/\/index.html//' | tail -"$n"
    ls -t dev_docs*/index.html | sed 's/\/index.html//' | tail -"$n" | xargs rm -rf
  fi
)

echo -n '-- Done at : ' ; date
