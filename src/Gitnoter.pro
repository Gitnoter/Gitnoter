#-------------------------------------------------
#
# Project created by MakeHui 2017-08-08T17:23:25
#
#-------------------------------------------------

QT       += core gui network printsupport xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gitnoter
TEMPLATE = app
CONFIG += c++11
ICON = Gitnoter.icns
RC_FILE = Gitnoter.rc
CODECFORTR = UTF-8

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

INCLUDEPATH += \
    $$PWD/controllers \
    $$PWD/view \
    $$PWD/models \
    $$PWD/helps

DEPENDPATH += \
    $$PWD/controllers \
    $$PWD/view \
    $$PWD/models \
    $$PWD/helps

SOURCES += \
    main.cpp \
    $$files("controllers/*.cpp", true) \
    $$files("models/*.cpp", true) \
    $$files("helps/*.cpp", true)


HEADERS += \
    gitnoter.h  \
    version.h \
    $$files("controllers/*.h", true) \
    $$files("models/*.h", true) \
    $$files("helps/*.h", true)

FORMS += \
    $$files("views/*.ui", true)

RESOURCES += \
    resources/resources.qrc

#
## DEPENDENCIES
#
include(3rdparty/3rdparty.pri)

