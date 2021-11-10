set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(HEADER_FILES
        QtAwesome.h
        QtAwesomeAnim.h)

set(SOURCE_FILES
        QtAwesome.cpp
        QtAwesomeAnim.cpp)

find_package(Qt5 REQUIRED Core Gui Widgets)
QT5_ADD_RESOURCES(RESOURCE_FILES
        QtAwesome.qrc)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED Core Gui Widgets)

add_library(QtAwesome ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})
target_link_libraries(QtAwesome Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(QtAwesome PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})