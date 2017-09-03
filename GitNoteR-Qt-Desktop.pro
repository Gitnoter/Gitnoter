#-------------------------------------------------
#
# Project created by QtCreator 2017-08-08T17:23:25
#
#-------------------------------------------------

QT       += core gui webenginewidgets webchannel sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitNoteR-Qt-Desktop
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/controllers \
    $$PWD/view \
    $$PWD/models

DEPENDPATH += $$PWD/controllers \
    $$PWD/view \
    $$PWD/models

SOURCES += \
        main.cpp \
        controllers/mainwindow.cpp \
        controllers/previewpage.cpp \
        controllers/document.cpp \
    controllers/categorieswidget.cpp \
    controllers/database.cpp \
    models/notemodel.cpp \
    controllers/tools.cpp \
    models/notetablemodel.cpp \
    models/categoriestablemodel.cpp \
    models/tagtablemodel.cpp \
    controllers/globals.cpp \
    models/configtablemodel.cpp \
    controllers/categorieslistcell.cpp

HEADERS += \
        controllers/mainwindow.h \
        controllers/previewpage.h \
        controllers/document.h \
    controllers/categorieswidget.h \
    controllers/database.h \
    models/notemodel.h \
    controllers/tools.h \
    controllers/sqlquerybuilder.h \
    models/notetablemodel.h \
    models/categoriestablemodel.h \
    models/tagtablemodel.h \
    controllers/globals.h \
    models/configtablemodel.h \
    controllers/categorieslistcell.h

FORMS += \
        views/mainwindow.ui \
    views/categorieswidget.ui \
    views/categorieslistcell.ui

RESOURCES += \
    resources/resources.qrc
