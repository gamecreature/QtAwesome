cmake_minimum_required(VERSION 2.9)

set(PROJECT_NAME QtAwesomeSample)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(${PROJECT_NAME})

set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
INCLUDE_DIRECTORIES(Qt5Widgets_INCLUDE_DIRS)
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../QtAwesome ${CMAKE_CURRENT_BINARY_DIR}/QtAwesome)
## or if QtAwesome is not out-of-tree dir, just:
#add_subdirectory(QtAwesome)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} QtAwesome)