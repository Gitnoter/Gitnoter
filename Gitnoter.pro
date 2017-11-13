#-------------------------------------------------
#
# Project created by MakeHui 2017-08-08T17:23:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gitnoter
TEMPLATE = app
CONFIG += c++11

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
    $$PWD/models \
    $$PWD/helps

DEPENDPATH += $$PWD/controllers \
    $$PWD/view \
    $$PWD/models \
    $$PWD/helps

SOURCES += \
    main.cpp \
    controllers/mainwindow.cpp  \
    controllers/categorylistwidget.cpp  \
    controllers/loginwidget.cpp \
    controllers/settingdialog.cpp \
    controllers/aboutdialog.cpp \
    controllers/lockdialog.cpp \
    controllers/notelistcellwidget.cpp \
    controllers/notelistsortpopupmenu.cpp \
    controllers/elidedlabel.cpp \
    controllers/markdowneditorwidget.cpp \
    controllers/tagcellwidget.cpp \
    controllers/messagedialog.cpp \
    controllers/grouptreewidget.cpp \
    controllers/notelistwidget.cpp \
    models/notemodel.cpp  \
    models/configmodel.cpp \
    models/groupmodel.cpp  \
    helps/globals.cpp  \
    helps/tools.cpp \
    helps/gitmanager.cpp \
    helps/thememanager.cpp \
    helps/notepreviewwidget.cpp

HEADERS += \
        controllers/mainwindow.h  \
    controllers/categorylistwidget.h  \
    controllers/loginwidget.h \
    controllers/settingdialog.h \
    controllers/aboutdialog.h \
    controllers/lockdialog.h \
    controllers/notelistcellwidget.h \
    controllers/notelistsortpopupmenu.h \
    controllers/elidedlabel.h \
    controllers/markdowneditorwidget.h \
    controllers/tagcellwidget.h \
    controllers/messagedialog.h \
    controllers/grouptreewidget.h \
    controllers/notelistwidget.h \
    models/notemodel.h  \
    models/configmodel.h \
    models/groupmodel.h \
    models/gitnoter.h  \
    helps/globals.h  \
    helps/tools.h \
    helps/gitmanager.h \
    helps/thememanager.h \
    helps/notepreviewwidget.h

FORMS += \
    views/mainwindow.ui \
    views/categorylistwidget.ui \
    views/loginwidget.ui \
    views/settingdialog.ui \
    views/aboutdialog.ui \
    views/lockdialog.ui \
    views/notelistcellwidget.ui \
    views/markdowneditorwidget.ui \
    views/tagcellwidget.ui \
    views/messagedialog.ui

RESOURCES += \
    resources/resources.qrc

#
## DEPENDENCIES
#
include(3rdparty/3rdparty.pri)

