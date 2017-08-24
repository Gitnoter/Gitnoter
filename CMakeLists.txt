cmake_minimum_required(VERSION 2.8.12)
project(GitNoteR-Qt-Desktop LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui WebEngineWidgets WebChannel Sql REQUIRED)

# include_directories(${PROJECT_SOURCE_DIR}/libraries/include)
# link_directories(${PROJECT_SOURCE_DIR}/libraries/lib)

include_directories(${PROJECT_SOURCE_DIR}/controllers)
include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/views)

# if (WIN32)
#     find_library(git2 libgit2.dll.a)
# elseif (APPLE)
#     find_library(git2 libgit2.dylib)
# elseif (UNIX)
# endif()

qt5_wrap_ui(UIS_HDRS
    views/mainwindow.ui
    views/categorieswidget.ui
)

add_executable(${PROJECT_NAME}
        main.cpp
        controllers/mainwindow.h
        controllers/previewpage.h
        controllers/document.h
        controllers/categorieswidget.h
        controllers/database.h
        controllers/appconfig.h
        models/notemodel.h
        models/notetablemodel.h
        models/categoriestablemodel.h
        models/tagtablemodel.h

        controllers/mainwindow.cpp
        controllers/previewpage.cpp
        controllers/document.cpp
        controllers/categorieswidget.cpp
        controllers/database.cpp
        controllers/appconfig.cpp
        models/notemodel.cpp
        models/notetablemodel.cpp
        models/categoriestablemodel.cpp
        models/tagtablemodel.cpp


        resources/resources.qrc

        ${UIS_HDRS}
        controllers/tools.cpp controllers/tools.h
        controllers/sqlquerybuilder.h)
# target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick git2)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::WebEngineWidgets Qt5::WebChannel Qt5::Sql)