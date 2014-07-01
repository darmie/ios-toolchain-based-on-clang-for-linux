#!/bin/bash

DIST_DIR=./cctools-855-ld64-236.3


LD64_FILE=ld64-236.3.tar.gz
CCTOOLS_FILE=cctools-855.tar.gz
LIBOBJC2_FILE=libobjc2-1.7.tgz

LD64_MD5=6018c9ad37d14a50015af04da395f8ac
CCTOOLS_MD5=480f9a872c3a846f0b2c4c3cf28a5408
LIBOBJC2_MD5=7cb75674c87113ea7c9c462502617fc4

LD64_URL=http://www.opensource.apple.com/tarballs/ld64/$LD64_FILE
CCTOOLS_URL=http://www.opensource.apple.com/tarballs/cctools/$CCTOOLS_FILE
LIBOBJC2_URL=http://download.gna.org/gnustep/$LIBOBJC2_FILE


echo "1.Download tarballs"
CUR_LD64_MD5=`md5sum ${LD64_FILE} |awk -F' ' '{print $1}'`
CUR_CCTOOLS_MD5=`md5sum ${CCTOOLS_FILE} |awk -F' ' '{print $1}'`
CUR_LIBOBJC2_MD5=`md5sum ${LIBOBJC2_FILE} |awk -F' ' '{print $1}'`

while [ "$CUR_LD64_MD5" != "$LD64_MD5" ];
do
    rm -rf $LD64_FILE
    proz -k=20  --no-curses $LD64_URL
    CUR_LD64_MD5=`md5sum ${LD64_FILE} |awk -F' ' '{print $1}'`
done

while [ "$CUR_CCTOOLS_MD5" != "$CCTOOLS_MD5" ];
do
    rm -rf $CCTOOLS_FILE
    proz -k=20  --no-curses $CCTOOLS_URL
    CUR_CCTOOLS_MD5=`md5sum ${CCTOOLS_FILE} |awk -F' ' '{print $1}'`
done

while [ "$CUR_LIBOBJC2_MD5" != "$LIBOBJC2_MD5" ];
do
    rm -rf $LIBOBJC2_FILE
    proz -k=20  --no-curses $LIBOBJC2_URL  >/dev/null 2>&1
    CUR_LIBOBJC2_MD5=`md5sum ${LIBOBJC2_FILE} |awk -F' ' '{print $1}'`
done

rm -rf ${DIST_DIR}
mkdir -p ${DIST_DIR}/{ld64,libobjc2}

if [ "`tar --help | grep -- --strip-components 2> /dev/null`" ]; then
    TARSTRIP=--strip-components
elif [ "`tar --help | grep bsdtar 2> /dev/null`" ]; then
    TARSTRIP=--strip-components
else
    TARSTRIP=--strip-path
fi
echo "2.Extract codes"
tar ${TARSTRIP}=1 -zxf ${CCTOOLS_FILE} -C ${DIST_DIR}
tar ${TARSTRIP}=1 -zxf ${LD64_FILE} -C ${DIST_DIR}/ld64 
tar ${TARSTRIP}=1 -zxf ${LIBOBJC2_FILE} -C ${DIST_DIR}/libobjc2

echo "3.Clean codes"
find . -name Makefile|xargs rm -rf
#rm -rf ${DIST_DIR}/{dyld,file,libdyld,mkshlib,profileServer,cctools-839,PB.project,RelNotes,libmacho}
rm -rf ${DIST_DIR}/{cbtlibs,dyld,file,libdyld,mkshlib,profileServer,cctools-839,efitools,PB.project,RelNotes,libmacho}
rm -rf ${DIST_DIR}/ld64/{ld64-134.9,ld64.xcodeproj,unit-tests}
rm -rf ${DIST_DIR}/libobjc2/{GNUmakefile,Makefile*}

echo "4.Removing #import"
find ${DIST_DIR} -type f -name \*.[ch] | xargs sed -i 's/^#import/#include/g'
echo "5.Removing __private_extern__"
find ${DIST_DIR} -type f -name \*.h | xargs sed -i 's/^__private_extern__/extern/g'

echo "6.Patch codes"
cat patches/cctools-855-ported.patch |patch -p1 -d ${DIST_DIR} 
cat patches/libobjc-remove-dispatch.patch |patch -p1 -d ${DIST_DIR} 
cat patches/ld64-236.3-ported.patch |patch -p1 -d ${DIST_DIR} 
cat patches/ld64-backward-support-for-llvm-gcc.patch |patch -p1 -d ${DIST_DIR} 

echo "7.Add external headers from Mac OS X"
tar cf - headers |tar ${TARSTRIP}=1 -xf - -C ${DIST_DIR}

echo "8.Add Automake project files"
tar cf - projectfiles |tar ${TARSTRIP}=1 -xf - -C ${DIST_DIR}

cat patches/github-tpoechtrager.patch|patch -p1 -d ${DIST_DIR}

echo "9.Clean a little bit"
find ${DIST_DIR} -name "*.orig"|xargs rm -rf
touch ${DIST_DIR}/{NEWS,AUTHORS,ChangeLog,README}
cp ${DIST_DIR}/APPLE_LICENSE ${DIST_DIR}/COPYING
chmod +x ${DIST_DIR}/autogen.sh

echo "10,Done"
echo ""
echo "Please run:"
echo ""
echo "\$./autogen.sh"
echo "\$CC=clang CXX=clang++ ./configure --prefix=/usr --target=arm-apple-darwin11"
echo "\$make"
echo "\$make install"
echo ""
echo "in ${DIST_DIR}"
