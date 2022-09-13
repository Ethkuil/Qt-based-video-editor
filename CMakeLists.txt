cmake_minimum_required(VERSION 3.15)

project(QtBasedVideoEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64")

find_package(Qt5
    COMPONENTS
        Core
        Gui
        Multimedia
        MultimediaWidgets
        Widgets
    REQUIRED)

aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/backend SRC_LIST)


add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST}) # hide console


target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Widgets
)
