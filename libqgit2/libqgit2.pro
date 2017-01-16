# qmake project file to build the static library libqgit2

include(../qmake/common.pri)

QT -= gui
TARGET = libqgit2
TEMPLATE = lib
CONFIG += staticlib C++11

# note: paths relative to _PRO_FILE_PWD_ which is the profile path of
# the subprojects in src/*
INCLUDEPATH += \
  qgit2 \
  ../libgit2/include \

# Currently qmake (version 3.0) does not remove a library
# target if the destination directory is not the build directory,
# so we try to add this. Unfortunately it may be non-platform-portable
# as we need to guess the real filename.
# See target 'distclean' in build/app-lib/Makefile
QMAKE_DISTCLEAN += $${DESTDIR}/$${QMAKE_PREFIX_STATICLIB}$${TARGET}.$${QMAKE_EXTENSION_STATICLIB}

*-g++ {
    # standard GNU c++ behaviour is to complain about this, but the
    # libgit2 headers we need are not prepared to fullfill all initializations
    QMAKE_CXXFLAGS += \
        -Wno-missing-field-initializers \

}

win32 {
    # currently we build a static library, so avoid declspec for dllimport/dllexport
    # just define an empty LIBQGIT2_EXPORT, see libgit2_config.h
    DEFINES += LIBQGIT2_EXPORT=""
}

HEADERS += \
  ./qgit2.h \
  ./qgit2/private/annotatedcommit.cpp \
  ./qgit2/private/buffer.cpp \
  ./qgit2/private/pathcodec.cpp \
  ./qgit2/private/remotecallbacks.cpp \
  ./qgit2/private/strarray.cpp \
  ./qgit2/qgitblob.cpp \
  ./qgit2/qgitcheckoutoptions.cpp \
  ./qgit2/qgitcherrypickoptions.cpp \
  ./qgit2/qgitcommit.cpp \
  ./qgit2/qgitconfig.cpp \
  ./qgit2/qgitcredentials.cpp \
  ./qgit2/qgitdatabasebackend.cpp \
  ./qgit2/qgitdatabase.cpp \
  ./qgit2/qgitdiff.cpp \
  ./qgit2/qgitdiffdelta.cpp \
  ./qgit2/qgitdifffile.cpp \
  ./qgit2/qgitexception.cpp \
  ./qgit2/qgitglobal.cpp \
  ./qgit2/qgitindex.cpp \
  ./qgit2/qgitindexentry.cpp \
  ./qgit2/qgitindexmodel.cpp \
  ./qgit2/qgitmergeoptions.cpp \
  ./qgit2/qgitobject.cpp \
  ./qgit2/qgitoid.cpp \
  ./qgit2/qgitrebase.cpp \
  ./qgit2/qgitrebaseoptions.cpp \
  ./qgit2/qgitref.cpp \
  ./qgit2/qgitremote.cpp \
  ./qgit2/qgitrepository.cpp \
  ./qgit2/qgitrevwalk.cpp \
  ./qgit2/qgitsignature.cpp \
  ./qgit2/qgitstatus.cpp \
  ./qgit2/qgitstatusentry.cpp \
  ./qgit2/qgitstatuslist.cpp \
  ./qgit2/qgitstatusoptions.cpp \
  ./qgit2/qgittag.cpp \
  ./qgit2/qgittree.cpp \
  ./qgit2/qgittreeentry.cpp \
  ./qgit2/credentials_p.h \
  ./qgit2/libqgit2_config.h \
  ./qgit2/private/annotatedcommit.h \
  ./qgit2/private/buffer.h \
  ./qgit2/private/pathcodec.h \
  ./qgit2/private/remotecallbacks.h \
  ./qgit2/private/strarray.h \
  ./qgit2/qgitblob.h \
  ./qgit2/qgitcheckoutoptions.h \
  ./qgit2/qgitcherrypickoptions.h \
  ./qgit2/qgitcommit.h \
  ./qgit2/qgitconfig.h \
  ./qgit2/qgitcredentials.h \
  ./qgit2/qgitdatabasebackend.h \
  ./qgit2/qgitdatabase.h \
  ./qgit2/qgitdiffdelta.h \
  ./qgit2/qgitdifffile.h \
  ./qgit2/qgitdiff.h \
  ./qgit2/qgitexception.h \
  ./qgit2/qgitglobal.h \
  ./qgit2/qgitindexentry.h \
  ./qgit2/qgitindex.h \
  ./qgit2/qgitindexmodel.h \
  ./qgit2/qgitmergeoptions.h \
  ./qgit2/qgitobject.h \
  ./qgit2/qgitoid.h \
  ./qgit2/qgitrebase.h \
  ./qgit2/qgitrebaseoptions.h \
  ./qgit2/qgitref.h \
  ./qgit2/qgitremote.h \
  ./qgit2/qgitrepository.h \
  ./qgit2/qgitrevwalk.h \
  ./qgit2/qgitsignature.h \
  ./qgit2/qgitstatusentry.h \
  ./qgit2/qgitstatus.h \
  ./qgit2/qgitstatuslist.h \
  ./qgit2/qgitstatusoptions.h \
  ./qgit2/qgittag.h \
  ./qgit2/qgittreeentry.h \
  ./qgit2/qgittree.h \

SOURCES += \
  ./qgit2/private/annotatedcommit.cpp \
  ./qgit2/private/buffer.cpp \
  ./qgit2/private/pathcodec.cpp \
  ./qgit2/private/remotecallbacks.cpp \
  ./qgit2/private/strarray.cpp \
  ./qgit2/qgitblob.cpp \
  ./qgit2/qgitcheckoutoptions.cpp \
  ./qgit2/qgitcherrypickoptions.cpp \
  ./qgit2/qgitcommit.cpp \
  ./qgit2/qgitconfig.cpp \
  ./qgit2/qgitcredentials.cpp \
  ./qgit2/qgitdatabasebackend.cpp \
  ./qgit2/qgitdatabase.cpp \
  ./qgit2/qgitdiff.cpp \
  ./qgit2/qgitdiffdelta.cpp \
  ./qgit2/qgitdifffile.cpp \
  ./qgit2/qgitexception.cpp \
  ./qgit2/qgitglobal.cpp \
  ./qgit2/qgitindex.cpp \
  ./qgit2/qgitindexentry.cpp \
  ./qgit2/qgitindexmodel.cpp \
  ./qgit2/qgitmergeoptions.cpp \
  ./qgit2/qgitobject.cpp \
  ./qgit2/qgitoid.cpp \
  ./qgit2/qgitrebase.cpp \
  ./qgit2/qgitrebaseoptions.cpp \
  ./qgit2/qgitref.cpp \
  ./qgit2/qgitremote.cpp \
  ./qgit2/qgitrepository.cpp \
  ./qgit2/qgitrevwalk.cpp \
  ./qgit2/qgitsignature.cpp \
  ./qgit2/qgitstatus.cpp \
  ./qgit2/qgitstatusentry.cpp \
  ./qgit2/qgitstatuslist.cpp \
  ./qgit2/qgitstatusoptions.cpp \
  ./qgit2/qgittag.cpp \
  ./qgit2/qgittree.cpp \
  ./qgit2/qgittreeentry.cpp \

OTHER_FILES += \
  ./tests/TestHelpers.h \
  ./tests/Checkout.cpp \
  ./tests/Clone.cpp \
  ./tests/Diff.cpp \
  ./tests/Fetch.cpp \
  ./tests/Init.cpp \
  ./tests/Push.cpp \
  ./tests/Rebase.cpp \
  ./tests/Repository.cpp \
  ./tests/Revision.cpp \
  ./tests/TestHelpers.cpp \
