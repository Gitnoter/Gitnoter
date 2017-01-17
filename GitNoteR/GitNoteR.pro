#-------------------------------------------------
#
# Project created by QtCreator 2017-01-16T15:07:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitNoteR
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

# 依赖项目
DEPENDENCY_LIBRARIES = libqgit2 libgit2

include(../qmake/includes.pri)

# note: paths relative to _PRO_FILE_PWD_ which is the profile path of
# the subprojects in src/*
INCLUDEPATH += ../libqgit2 ../libgit2/include

# Currently qmake (version 3.0) does not remove a library
# target if the destination directory is not the build directory,
# so we try to add this. Unfortunately it may be non-platform-portable
# as we need to guess the real filename.
# See target 'distclean' in build/app-lib/Makefile
QMAKE_DISTCLEAN += $${DESTDIR}/$${QMAKE_PREFIX_STATICLIB}$${TARGET}.$${QMAKE_EXTENSION_STATICLIB}

*-g++ {
    # standard GNU c++ behaviour is to complain about this, but the
    # libgit2 headers we need are not prepared to fullfill all initializations
    QMAKE_CXXFLAGS += -Wno-missing-field-initializers
}

win32 {
    # currently we build a static library, so avoid declspec for dllimport/dllexport
    # just define an empty LIBQGIT2_EXPORT, see libgit2_config.h
    DEFINES += LIBQGIT2_EXPORT=""

    # we need the windows socket library for libqgit2/libgit2
    LIBS += -lws2_32
}
