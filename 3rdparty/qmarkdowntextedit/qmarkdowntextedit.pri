INCLUDEPATH += $$PWD/

QT       += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    $$PWD/markdownhighlighter.cpp \
    $$PWD/qmarkdowntextedit.cpp \
    $$PWD/qtexteditsearchwidget.cpp

HEADERS += \
    $$PWD/markdownhighlighter.h \
    $$PWD/qmarkdowntextedit.h \
    $$PWD/qtexteditsearchwidget.h

RESOURCES += \
    $$PWD/media.qrc

FORMS += $$PWD/qtexteditsearchwidget.ui


#include($$PWD/qmarkdowntextedit-headers.pri)
#include($$PWD/qmarkdowntextedit-sources.pri)
