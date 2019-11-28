cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Xml XmlPatterns REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/hoedown)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/json)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qtinyaes )
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/UGlobalHotkey)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qt-google-analytics)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/LibGit2Wrapper)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qBreakpad/handler)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/qkeysequencewidget/src)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/include)

file(GLOB_RECURSE SOURCES_UIS ${PROJECT_SOURCE_DIR}/views/*.ui)
qt5_wrap_ui(UI_HEADERS ${SOURCES_UIS})

include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/views)
include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/helps)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty)

#aux_source_directory(${PROJECT_SOURCE_DIR}/controllers SOURCES_CONTROLLERS)
#aux_source_directory(${PROJECT_SOURCE_DIR}/models SOURCES_MODELS)
#aux_source_directory(${PROJECT_SOURCE_DIR}/helps SOURCES_HELPS)

add_executable(${PROJECT_NAME}
    main.cpp
    gitnoter.h
    version.h
    ${UI_HEADERS}
#    ${SOURCES_CONTROLLERS} ${SOURCES_MODELS} ${SOURCES_HELPS}
    resources/resources.qrc)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Xml Qt5::XmlPatterns)