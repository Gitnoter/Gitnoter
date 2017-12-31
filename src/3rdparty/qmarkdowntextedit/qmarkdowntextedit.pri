INCLUDEPATH += $$PWD/

QT       += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    $$PWD/markdownhighlighter.cpp \
    $$PWD/qmarkdowntextedit.cpp \
    $$PWD/qplaintexteditsearchwidget.cpp

HEADERS += \
    $$PWD/markdownhighlighter.h \
    $$PWD/qmarkdowntextedit.h \
    $$PWD/qplaintexteditsearchwidget.h

RESOURCES += \
    $$PWD/media.qrc

FORMS += $$PWD/qplaintexteditsearchwidget.ui


#include($$PWD/qmarkdowntextedit-headers.pri)
#include($$PWD/qmarkdowntextedit-sources.pri)
