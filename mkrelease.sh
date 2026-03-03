#!/bin/sh
#
# Script for generating a release archive.
#

HOST="${1:-m68k-amigaos}"

if [ "$HOST" = "m68k-amigaos" ]; then
  make all
else
  CPU=`echo "${HOST}" | cut -d'-' -f1`
  make -f makefile.aros CPU=${CPU} all
fi;

DESTDIR='tmp'

rm -rf ${DESTDIR}
mkdir -p ${DESTDIR}/ntfs3g/L

cp -p COPYING ${DESTDIR}/ntfs3g
cp -p releasenotes ${DESTDIR}/ntfs3g

if [ "$HOST" = "m68k-amigaos" ]; then
  cp -p README ${DESTDIR}/ntfs3g
  cp -p bin/NTFileSystem3G.000 ${DESTDIR}/ntfs3g/L
  cp -p bin/NTFileSystem3G.020 ${DESTDIR}/ntfs3g/L
  cp -p bin/NTFileSystem3G.060 ${DESTDIR}/ntfs3g/L
else
  cp -p README-AROS ${DESTDIR}/ntfs3g/README
  cp -p bin/ntfs3g-handler.${CPU} ${DESTDIR}/ntfs3g/L/ntfs3g-handler
fi;

cp -p icons/def_drawer.info ${DESTDIR}/ntfs3g.info
cp -p icons/def_doc.info ${DESTDIR}/ntfs3g/README.info
cp -p icons/def_doc.info ${DESTDIR}/ntfs3g/COPYING.info
cp -p icons/def_doc.info ${DESTDIR}/ntfs3g/releasenotes.info

rm -f ntfs3g.${HOST}.7z
7za u ntfs3g.${HOST}.7z ./${DESTDIR}/*
echo "ntfs3g.${HOST}.7z created"

rm -rf ${DESTDIR}

