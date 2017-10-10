cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui WebEngineWidgets WebChannel Sql REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/libraries/include)
link_directories(${PROJECT_SOURCE_DIR}/libraries/lib)

include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/views)
include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/helps)

qt5_wrap_ui(UIS_HDRS
        views/mainwindow.ui
        views/categorieswidget.ui
        views/categorieslistcell.ui
        views/tagslistcell.ui
        views/tagswidget.ui
        views/loginwidget.ui
        views/settingdialog.ui
        views/aboutdialog.ui)

add_executable(${PROJECT_NAME}
        ${UIS_HDRS}
        main.cpp
        resources/resources.qrc

        controllers/mainwindow.h            controllers/mainwindow.cpp
        controllers/categorieswidget.h      controllers/categorieswidget.cpp
        controllers/categorieslistcell.h    controllers/categorieslistcell.cpp
        controllers/tagslistcell.h          controllers/tagslistcell.cpp
        controllers/tagswidget.h            controllers/tagswidget.cpp
        controllers/loginwidget.h           controllers/loginwidget.cpp
        controllers/settingdialog.h         controllers/settingdialog.cpp
        controllers/aboutdialog.h           controllers/aboutdialog.cpp

        models/notemodel.h          models/notemodel.cpp
        models/contentmodel.h       models/contentmodel.cpp
        models/categoriesmodel.h    models/categoriesmodel.cpp
        models/tagsmodel.h          models/tagsmodel.cpp
        models/configmodel.h        models/configmodel.cpp

        helps/previewpage.h     helps/previewpage.cpp
        helps/document.h        helps/document.cpp
        helps/globals.h         helps/globals.cpp
        helps/tools.h           helps/tools.cpp
        helps/gitmanager.h      helps/gitmanager.cpp
        helps/json.h            helps/json.cpp
        helps/qtinyaes/aes.h    helps/qtinyaes/aes.c
        helps/qtinyaes/qtinyaes.h helps/qtinyaes/qtinyaes.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::WebEngineWidgets Qt5::WebChannel git2)