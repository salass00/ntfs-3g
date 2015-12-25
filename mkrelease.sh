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

cp -p ntfs3g.readme ${DESTDIR}/ntfs3g-${NUMVERS}
cp -p LICENSE.APL ${DESTDIR}/ntfs3g-${NUMVERS}
cp -p releasenotes ${DESTDIR}/ntfs3g-${NUMVERS}
cp -p ntfs3g-handler ${DESTDIR}/ntfs3g-${NUMVERS}/L

sed -i "s/^Version:      xx.xx/Version:      ${NUMVERS}/" ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme
sed -i "s/^Architecture: xxx/Architecture: ${HOST}/" ${DESTDIR}/ntfs3g-${NUMVERS}/ntfs3g.readme

rm -f ntfs3g.${HOST}.7z
7za u ntfs3g.${HOST}.7z ./${DESTDIR}/ntfs3g-${NUMVERS}

rm -rf ${DESTDIR}

echo "ntfs3g.${HOST}.7z created"

