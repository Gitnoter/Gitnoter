cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Xml XmlPatterns REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/hoedown HOEDOWN)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/include LIBGIT2)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/json JSON)
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/qtinyaes QTINYAES)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/libgit2/lib)
find_library(git2 libgit2)

include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/views)
include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/helps)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty)

qt5_wrap_ui(UIS_HDRS
        views/mainwindow.ui
        views/categorylistwidget.ui
        views/loginwidget.ui
        views/settingdialog.ui
        views/aboutdialog.ui
        views/lockdialog.ui
        views/notelistcellwidget.ui
        views/markdowneditorwidget.ui
        views/tagcellwidget.ui
        views/messagedialog.ui
        views/menubar.ui
        views/importnotedialog.ui)

add_executable(${PROJECT_NAME}
        ${UIS_HDRS}
        main.cpp
        gitnoter.h
        version.h
        resources/resources.qrc

        controllers/mainwindow.h            controllers/mainwindow.cpp
        controllers/categorylistwidget.h    controllers/categorylistwidget.cpp
        controllers/loginwidget.h           controllers/loginwidget.cpp
        controllers/settingdialog.h         controllers/settingdialog.cpp
        controllers/aboutdialog.h           controllers/aboutdialog.cpp
        controllers/lockdialog.h            controllers/lockdialog.cpp
        controllers/notelistcellwidget.h    controllers/notelistcellwidget.cpp
        controllers/notelistsortpopupmenu.h controllers/notelistsortpopupmenu.cpp
        controllers/elidedlabel.h           controllers/elidedlabel.cpp
        controllers/markdowneditorwidget.h  controllers/markdowneditorwidget.cpp
        controllers/tagcellwidget.h         controllers/tagcellwidget.cpp
        controllers/messagedialog.h         controllers/messagedialog.cpp
        controllers/notelistwidget.h        controllers/notelistwidget.cpp
        controllers/grouptreewidget.h       controllers/grouptreewidget.cpp
        controllers/menubar.h               controllers/menubar.cpp
        controllers/navigationbarwidget.h   controllers/navigationbarwidget.cpp
        controllers/importnotedialog.h      controllers/importnotedialog.cpp

        models/notemodel.h          models/notemodel.cpp
        models/configmodel.h        models/configmodel.cpp
        models/groupmodel.h         models/groupmodel.cpp

        helps/globals.h             helps/globals.cpp
        helps/tools.h               helps/tools.cpp
        helps/gitmanager.h          helps/gitmanager.cpp
        helps/thememanager.h        helps/thememanager.cpp
        helps/notepreviewwidget.h   helps/notepreviewwidget.cpp

        ${HOEDOWN} ${LIBGIT2} ${JSON} ${QTINYAES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Xml Qt5::XmlPatterns git2)