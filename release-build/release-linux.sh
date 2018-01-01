#! /bin/bash

rm -rf ./release/Gitnoter
mkdir -p ./release/Gitnoter
cp -rf ../build-Gitnoter-Desktop_Qt_5_8_0_GCC_64bit-Release/Gitnoter ./release/Gitnoter/

i=0
cat ../src/version.h | while read line
do
    if [[ ${i} == 9 ]]; then
        VERSION=`echo ${line} | sed 's/.*VER_PRODUCTVERSION_STR *"\([0-9.-beta]*\)".*/\1/g'`
        linuxdeployqt ./release/Gitnoter/Gitnoter
        cp ../src/3rdparty/libgit2/lib/libgit2.so ./release/Gitnoter/lib/libgit2.so.24
        echo "app verson: ${VERSION}"
        cd release
        tar -cvzf Gitnoter-linux-${VERSION}.tar.gz Gitnoter
    fi
    let i=i+1
done
