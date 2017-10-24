#-------------------------------------------------
#
# Project created by QtCreator 2017-08-08T17:23:25
#
#-------------------------------------------------

QT       += core gui webenginewidgets webchannel

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
    controllers/categorieswidget.cpp  \
    controllers/categorieslistcell.cpp  \
    controllers/tagslistcell.cpp  \
    controllers/tagswidget.cpp  \
    controllers/loginwidget.cpp \
    controllers/settingdialog.cpp \
    controllers/aboutdialog.cpp \
    controllers/lockdialog.cpp \
    controllers/notelistcellwidget.cpp \
    controllers/notelistsortpopupmenu.cpp \
    controllers/elidedlabel.cpp \
    models/contentmodel.cpp  \
    models/notemodel.cpp  \
    models/categoriesmodel.cpp  \
    models/tagsmodel.cpp  \
    models/configmodel.cpp  \
    helps/previewpage.cpp  \
    helps/document.cpp  \
    helps/globals.cpp  \
    helps/tools.cpp \
    helps/gitmanager.cpp \
    helps/thememanager.cpp

HEADERS += \
        controllers/mainwindow.h  \
    controllers/categorieswidget.h  \
    controllers/categorieslistcell.h  \
    controllers/tagslistcell.h  \
    controllers/tagswidget.h  \
    controllers/loginwidget.h \
    controllers/settingdialog.h \
    controllers/aboutdialog.h \
    controllers/lockdialog.h \
    controllers/notelistcellwidget.h \
    controllers/notelistsortpopupmenu.h \
    controllers/elidedlabel.h \
    models/contentmodel.h  \
    models/notemodel.h  \
    models/categoriesmodel.h  \
    models/tagsmodel.h  \
    models/configmodel.h  \
    helps/previewpage.h  \
    helps/document.h  \
    helps/globals.h  \
    helps/tools.h \
    helps/gitmanager.h \
    helps/thememanager.h

FORMS += \
    views/mainwindow.ui \
    views/categorieswidget.ui \
    views/categorieslistcell.ui \
    views/tagslistcell.ui \
    views/tagswidget.ui \
    views/loginwidget.ui \
    views/settingdialog.ui \
    views/aboutdialog.ui \
    views/lockdialog.ui \
    views/notelistcellwidget.ui

RESOURCES += \
    resources/resources.qrc

#
## DEPENDENCIES
#
include(3rdparty/3rdparty.pri)

