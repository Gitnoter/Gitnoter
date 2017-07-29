cmake_minimum_required(VERSION 2.8.12)

project(GitNoteR-Qt-Desktop LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/libraries/include)
link_directories("${PROJECT_SOURCE_DIR}/libraries/lib")

find_library(git2 libgit2.dylib)

add_executable(${PROJECT_NAME}
        "controllers/GitManager.h"
        "libraries/include/git2.h"

        "controllers/main.cpp"
        "controllers/GitManager.cpp"

        "views/qml.qrc"
)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick git2)
