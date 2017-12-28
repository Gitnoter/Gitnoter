#include(uglobalhotkey-headers.pri)
#include(uglobalhotkey-sources.pri)
#include(uglobalhotkey-libs.pri)

!win32 {
    QT += gui-private

    INCLUDEPATH += $$PWD/

    HEADERS += \
        $$PWD/ukeysequence.h \
        $$PWD/uglobalhotkeys.h \
        $$PWD/uexception.h \
        $$PWD/hotkeymap.h \
        $$PWD/uglobal.h

    SOURCES += \
        $$PWD/ukeysequence.cpp \
        $$PWD/uglobalhotkeys.cpp \
        $$PWD/uexception.cpp

    # Linking options for different platforms

    linux: LIBS += -lxcb -lxcb-keysyms
    mac: LIBS += -framework Carbon
}

win32 {
    LIBS += -L$$PWD/build/ -lUGlobalHotkey
    INCLUDEPATH += $$PWD/build
    DEPENDPATH += $$PWD/build
}
