#! /bin/bash

mkdir -p release/
rm -rf ./release/Gitnoter.app
cp -rf ../build-Gitnoter-Desktop_Qt_5_9_3_clang_64bit-Release/Gitnoter.app ./release/

i=0
cat ../src/version.h | while read line
do
    if [[ ${i} == 9 ]]; then
        VERSION=`echo ${line} | sed 's/.*VER_PRODUCTVERSION_STR *"\([0-9.-beta]*\)".*/\1/g'`
        macdeployqt ./release/Gitnoter.app
        appdmg ./package-osx/package.json ./release/Gitnoter-osx-v${VERSION}.dmg
    fi
    let i=i+1
done
