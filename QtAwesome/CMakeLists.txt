cmake_minimum_required(VERSION 3.16)
project(QtAwesome)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(QtAwesome
  QtAwesome.cpp
  QtAwesomeAnim.cpp
  QtAwesomeFree.qrc
)

target_include_directories(QtAwesome
  INTERFACE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(QtAwesome PUBLIC
  Qt${QT_VERSION_MAJOR}::Widgets
)
