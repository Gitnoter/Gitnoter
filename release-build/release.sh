#! /bin/bash

rm -rf Gitnoter.app
cp -rf ../build-Gitnoter-Desktop_Qt_5_9_3_clang_64bit-Release/Gitnoter.app ./

i=0
cat ../src/version.h | while read line
do
    if [[ ${i} == 9 ]]; then
        VERSION=`echo ${line} | sed 's/.*VER_PRODUCTVERSION_STR *"\([0-9.-beta]*\)".*/\1/g'`
        macdeployqt Gitnoter.app
        appdmg package.json ./release/Gitnoter-${VERSION}.dmg
    fi
    let i=i+1
done
