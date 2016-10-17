#!/bin/sh
#
# Script for generating a release archive.
#

HOST="${1:-i386-aros}"

make HOST=${HOST} clean

make HOST=${HOST}

DESTDIR='tmp'
FULLVERS=`version ntfs3g-handler`
NUMVERS=`echo "${FULLVERS}" | cut -d' ' -f2`

rm -rf ${DESTDIR}
mkdir -p ${DESTDIR}/ntfs3g-${NUMVERS}/L

cp -p COPYING ${DESTDIR}/ntfs3g-${NUMVERS}
cp -p releasenotes ${DESTDIR}/ntfs3g-${NUMVERS}
cp -p ntfs3g-handler ${DESTDIR}/ntfs3g-${NUMVERS}/L

echo "Short:        A port of Tuxera's NTFS-3G file system" > ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Author:       Fredrik Wikstrom" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Uploader:     Fredrik Wikstrom <fredrik@a500.org>" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Type:         disk/misc" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Version:      ${NUMVERS}" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Requires:     util/libs/filesysbox.${HOST}.lha" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "Architecture: ${HOST}" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
echo "" >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
cat README >> ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme

rm -f ntfs3g.${HOST}.7z
7za u ntfs3g.${HOST}.7z ./${DESTDIR}/ntfs3g-${NUMVERS}

rm -rf ${DESTDIR}

echo "ntfs3g.${HOST}.7z created"

