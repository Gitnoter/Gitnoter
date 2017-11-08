cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/views)
include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/helps)

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
        views/messagedialog.ui)

add_executable(${PROJECT_NAME}
        ${UIS_HDRS}
        main.cpp
        resources/resources.qrc

        controllers/mainwindow.h            controllers/mainwindow.cpp
        controllers/categorylistwidget.h controllers/categorylistwidget.cpp
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

        models/notemodellist.h      models/notemodellist.cpp
        models/categorymodellist.h  models/categorymodellist.cpp
        models/tagmodellist.h       models/tagmodellist.cpp
        models/configmodel.h        models/configmodel.cpp
        models/groupmodel.h models/groupmodel.cpp

        helps/globals.h         helps/globals.cpp
        helps/tools.h           helps/tools.cpp
        helps/gitmanager.h      helps/gitmanager.cpp
        helps/thememanager.h        helps/thememanager.cpp
        helps/notepreviewwidget.h   helps/notepreviewwidget.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)