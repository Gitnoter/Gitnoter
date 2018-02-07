message("BREAKPAD_crash_handler_attached")


CONFIG -= app_bundle
CONFIG += debug_and_release warn_on
CONFIG += thread exceptions rtti stl

# without c++11 & AppKit library compiler can't solve address for symbols
CONFIG += c++11
macx: LIBS += -framework AppKit

INCLUDEPATH += $$PWD/handler/

HEADERS += \
    $$PWD/handler/QBreakpadHandler.h \
    $$PWD/handler/QBreakpadHttpUploader.h

LIBS += \
    -L$$PWD/handler -lqBreakpad
