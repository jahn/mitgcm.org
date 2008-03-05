#! /usr/bin/env bash

# $Header: /u/gcmpack/mitgcm.org/scripts/bld_manual.sh,v 1.2 2008/03/03 20:32:00 jmc Exp $

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
ADDRERR='jmc@ocean.mit.edu'

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
    make All > ../../../build.log 2>&1
)
echo -n '-- Finish building manual at : ' ; date

newbld="dev_docs_"`date +%Y%m%d`"_"`date +%H%M`
if test -f  scratch/dev_docs/index.html ; then
    echo " mv scratch/dev_docs $OUTDIR/$newbld"
    mv scratch/dev_docs $OUTDIR/$newbld
else
  if test -e scratch/dev_docs ; then
    echo " mv scratch/dev_docs scratch/$newbld"
    mv scratch/dev_docs scratch/$newbld
  fi
    echo "========================================================"
    echo "error running '"`basename $0`"' at : "`date` | tee tmp.$$
    echo " no file 'scratch/dev_docs/index.html' !" | tee -a tmp.$$
    echo " ==> Incomplete build" | tee -a tmp.$$
    echo "" | tee -a tmp.$$
    echo "-> tail -15 build.log : >->->->->->->->->->->->->" | tee -a tmp.$$
    tail -15 build.log | tee -a tmp.$$
    echo "<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<" | tee -a tmp.$$
    if test "x$ADDRERR" != x ; then
        mail -s `basename $0`' Error' $ADDRERR < tmp.$$
    fi
    rm -f tmp.$$
    exit 2
fi

(
  cd $OUTDIR
  echo "-- Install latest in dir: "`pwd`
  test -e latest  &&  rm -f latest
  echo " ln -s `ls -td dev_docs* | head -1` latest"
  ln -s `ls -td dev_docs* | head -1` latest
  n=$(( `ls dev_docs*/index.html | wc -l` - 7 ))
  if test $n -gt 0 ; then
    echo -n ' remove dir: ' 
    ls -t dev_docs*/index.html | sed 's/\/index.html//' | tail -"$n"
    ls -t dev_docs*/index.html | sed 's/\/index.html//' | tail -"$n" | xargs rm -rf
  fi
)

echo -n '-- Done at : ' ; date
