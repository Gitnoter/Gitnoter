#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T16:54:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitNoteR-Demo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

macx {
    LIBS += -L/usr/local/lib/ -lqgit2

    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include
}
