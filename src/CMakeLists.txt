cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop)

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Xml XmlPatterns REQUIRED)

#
# https://github.com/hoedown/hoedown
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/hoedown)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/hoedown HOEDOWN)

#
# https://github.com/nlohmann/json
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/json)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/json JSON)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qtinyaes)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/qtinyaes QTINYAES)

#
# https://github.com/falceeffect/UGlobalHotkey
#
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/UGlobalHotkey)
#aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/UGlobalHotkey UGlOBLEHOTKEY)

#
# https://github.com/HSAnet/qt-google-analytics
# Qt5 classes for providing Google analytics usage in a Qt/QtQuick application
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qt-google-analytics)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/qt-google-analytics QtGoogleAnalytics)

#
# https://github.com/stsoor/LibGit2Wrapper
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/LibGit2Wrapper)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/LibGit2Wrapper LibGit2Wrapper)

#
# https://github.com/buzzySmile/qBreakpad
# Cross-platform Qt crash handler, implemented as wrapper around google-breakpad
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qBreakpad/handler)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/qBreakpad/handler)
find_library(qBreakpad libqBreakpad)
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework AppKit")
    execute_process(COMMAND install_name_tool -id @executable_path/../3rdparty/libgit2/lib/libgit2.24.dylib ${PROJECT_SOURCE_DIR}/3rdparty/libgit2/lib/libgit2.dylib)
endif()

#
# https://github.com/libgit2/libgit2
# A cross-platform, linkable library implementation of Git that you can use in your application.
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/lib)
find_library(git2 libigt2)

#
# https://github.com/pbek/qmarkdowntextedit
# markdown editor
#
set(SOURCES_QMARKDOWNTEXTEDIT
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/markdownhighlighter.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/qmarkdowntextedit.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/markdownhighlighter.h
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/qmarkdowntextedit.h
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.h
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/media.qrc)
set(UIS_QMARKDOWNTEXTEDIT
    ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.ui)

#
# https://github.com/DOOMer/qkeysequencewidget
# Qt widget for create key sequences
#
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/src)
set(SOURCES_QKEYSEQUENCEWIDGET
    ${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/src/qkeysequencewidget.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/src/qkeysequencewidget.h
    ${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/src/qkeysequencewidget_p.h
    ${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/qkeysequencewidget.qrc)

#
# App sources
#
include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/helps)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty)

aux_source_directory(${PROJECT_SOURCE_DIR}/controllers SOURCES_CONTROLLERS)
aux_source_directory(${PROJECT_SOURCE_DIR}/models SOURCES_MODELS)
aux_source_directory(${PROJECT_SOURCE_DIR}/helps SOURCES_HELPS)

file(GLOB_RECURSE UIS_APP ${PROJECT_SOURCE_DIR}/views/*.ui)

#
# Build Qt UIs to headers
#
qt5_wrap_ui(HEADERS_UIS
    ${UIS_APP}
    ${UIS_QMARKDOWNTEXTEDIT})

add_executable(${PROJECT_NAME}
    main.cpp
    gitnoter.h
    version.h
    ${HEADERS_UIS}
    ${SOURCES_CONTROLLERS}
    ${SOURCES_MODELS}
    ${SOURCES_HELPS}
    ${HOEDOWN} ${JSON} ${QTINYAES} ${QtGoogleAnalytics} ${LibGit2Wrapper}
    ${SOURCES_QMARKDOWNTEXTEDIT}
    ${SOURCES_QKEYSEQUENCEWIDGET}
    resources/resources.qrc)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::PrintSupport
    Qt5::Xml
    Qt5::XmlPatterns
    git2
    qBreakpad)